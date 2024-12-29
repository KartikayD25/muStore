void sendWithZeroCopy(int sockfd, std::vector<SingleSimpleFieldValue*>& values_) {
    int iov_len = 1 + values_.size();  // Adjust size for fields
    struct iovec iov[iov_len];

    std::vector<std::shared_ptr<void>> buffer_owners;  // Hold shared_ptrs to ensure memory safety
    std::vector<std::atomic<bool>> in_use(iov_len);    // Atomic flags to track usage

    int iov_idx = 0;
    size_t offset = 0;

    // Initialize first iovec entry (e.g., bitmap or fixed field)
    int bitmap_ = 0;
    iov[iov_idx].iov_base = &bitmap_;
    iov[iov_idx++].iov_len = sizeof(bitmap_);
    offset += sizeof(bitmap_);

    // Populate the iovec array with field values
    for (auto& field_val : values_) {
        const Symbol& sym = field_val->getSymbol();
        uint32_t id = sym.id_;
        if (!isPresent(id)) {
            continue;
        }

        // Skip complex or repeated types for simplicity
        if (sym.type_ == UserTypes::TYPE_MESSAGE || TypeHelper::isComplexType(sym.type_) || sym.repeated_) {
            continue;
        }

        auto value = dynamic_cast<SingleSimpleFieldValue*>(field_val);
        void* base_ptr = value->getBasePtr();
        size_t size = TypeHelper::typeSize(sym.type_);

        // Wrap base_ptr in a shared_ptr and add to buffer_owners
        std::shared_ptr<void> buffer_owner(base_ptr, [](void*) {});  // Dummy deleter for illustration
        buffer_owners.push_back(buffer_owner);

        iov[iov_idx].iov_base = base_ptr;
        iov[iov_idx].iov_len = size;
        in_use[iov_idx].store(true, std::memory_order_release);  // Mark buffer as in use
        ++iov_idx;
        offset += size;
    }

    // Set up and send the message using sendmsg with MSG_ZEROCOPY
    struct msghdr msg = {};
    msg.msg_iov = iov;
    msg.msg_iovlen = iov_len;

    if (sendmsg(sockfd, &msg, MSG_ZEROCOPY) == -1) {
        perror("sendmsg failed");
        return;
    }

    // Start a thread to wait for DMA completion notification
    std::thread([&]() {
        struct msghdr completion_msg = {};
        ssize_t ret;
        while ((ret = recvmsg(sockfd, &completion_msg, MSG_ERRQUEUE)) > 0) {
            struct cmsghdr* cmsg = CMSG_FIRSTHDR(&completion_msg);
            if (cmsg && cmsg->cmsg_level == SOL_IP && cmsg->cmsg_type == SO_EE_ORIGIN_ZEROCOPY) {
                // Release buffers safely after DMA completion
                buffer_owners.clear(); // Shared_ptrs deallocate memory upon clearing
                for (auto& flag : in_use) {
                    flag.store(false, std::memory_order_release);  // Mark as not in use
                }
                break;
            }
        }
    }).detach();
}