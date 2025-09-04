.global _ZN3PCB13contextSwitchEPNS_7ContextES1_
.type _ZN3PCB13contextSwitchEPNS_7ContextES1_, @function
_ZN3PCB13contextSwitchEPNS_7ContextES1_:
    # a0 - old->context
    # a1 - old->context
    sd ra, 0*8(a0) # old->context->ra
    sd sp, 1*8(a0) # old->context->sp

    ld ra, 0*8(a1)
    ld sp, 1*8(a1)
    ret
