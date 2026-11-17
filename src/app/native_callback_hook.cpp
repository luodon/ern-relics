#include "app/native_callback_hook.hpp"
#include <Windows.h>

namespace ern {
std::optional<std::string> NativeCallbackHook::install(MemoryRead read, Address slot,
                                                       Address original, Address callback) {
    if (binding_)
        return "装备更新回调已安装。";
    if (!slot || slot % alignof(Address) != 0 || !original || !callback ||
        read_value<Address>(read, slot) != original)
        return "不支持当前装备更新回调布局，编辑已锁定。";
    DWORD protection{};
    if (!VirtualProtect(reinterpret_cast<void *>(slot), sizeof(Address), PAGE_READWRITE,
                        &protection))
        return "无法访问装备更新回调，编辑已锁定。";
    auto *pointer = reinterpret_cast<PVOID volatile *>(slot);
    const auto previous = InterlockedCompareExchangePointer(
        pointer, reinterpret_cast<void *>(callback), reinterpret_cast<void *>(original));
    DWORD ignored{};
    if (!VirtualProtect(reinterpret_cast<void *>(slot), sizeof(Address), protection, &ignored)) {
        // Roll back only our pointer; never overwrite a concurrent owner's replacement.
        InterlockedCompareExchangePointer(pointer, reinterpret_cast<void *>(original),
                                          reinterpret_cast<void *>(callback));
        VirtualProtect(reinterpret_cast<void *>(slot), sizeof(Address), protection, &ignored);
        return "无法恢复装备回调的内存保护，编辑已锁定。";
    }
    if (reinterpret_cast<Address>(previous) != original)
        return "安装过程中装备更新回调发生变化，编辑已锁定。";
    binding_ = Binding{std::move(read), slot, callback};
    return std::nullopt;
}

std::optional<std::string> NativeCallbackHook::error() const {
    if (!binding_)
        return "装备更新回调未安装，编辑已锁定。";
    const auto observed = read_value<Address>(binding_->read, binding_->slot);
    if (!observed)
        return "无法读取装备更新回调，编辑已锁定。";
    if (*observed != binding_->callback)
        return "装备更新回调已被替换，编辑已锁定。请重启 Nightreign。";
    return std::nullopt;
}
} // namespace ern
