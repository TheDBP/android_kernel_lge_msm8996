ccflags-y += -I$(src)			# needed for trace events

obj-$(CONFIG_ANDROID_BINDER_IPC)	+= binder.o binder_alloc.o

# binder_alloc.c carries a deliberate type mismatch, not an accident: 83b0b662b5e8 partially
# reverted binder to get the IR blaster working, which left binder_update_page_range() treating
# alloc->pages as the pre-LRU "struct page **" while binder_alloc.h kept struct binder_lru_page,
# and 63626be73f11 ("Fix a myriad of warnings") settled the mismatch in the declaration rather
# than the body. GCC warned and shipped; clang makes -Wincompatible-pointer-types fatal.
#
# Suppressed for this one object rather than fixed, because writing the function back to upstream
# LRU form would re-enable exactly the code path that revert removed, and the IR blaster is the
# thing it was removed for. Scoped to the file so no other driver loses the diagnostic.
CFLAGS_binder_alloc.o += -Wno-incompatible-pointer-types
obj-$(CONFIG_ANDROID_BINDER_IPC_SELFTEST) += binder_alloc_selftest.o
