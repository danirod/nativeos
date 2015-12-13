/* ISR definitions. */

#ifndef KERNEL_ISRDEF_H_
#define KERNEL_ISRDEF_H_

/*
 * How many interrupts are supported at this moment:
 * o Interrupts 0-31 are reserved for the system.
 * o Interrupts 32-48 are reserved for hardware (PIC, keyboard, timer...)
 * o Still have to decide an interrupt for system calls.
 */
#define INTERRUPTS 48

/*
 * These are my interrupt entrypoints. This is ugly, but it has to be done
 * since every interrupt is actually a different function (or memory address)
 * that has to be given to the IDT Table.
 */
extern void isr_0(void);
extern void isr_1(void);
extern void isr_2(void);
extern void isr_3(void);
extern void isr_4(void);
extern void isr_5(void);
extern void isr_6(void);
extern void isr_7(void);
extern void isr_8(void);
extern void isr_9(void);
extern void isr_10(void);
extern void isr_11(void);
extern void isr_12(void);
extern void isr_13(void);
extern void isr_14(void);
extern void isr_15(void);
extern void isr_16(void);
extern void isr_17(void);
extern void isr_18(void);
extern void isr_19(void);
extern void isr_20(void);
extern void isr_21(void);
extern void isr_22(void);
extern void isr_23(void);
extern void isr_24(void);
extern void isr_25(void);
extern void isr_26(void);
extern void isr_27(void);
extern void isr_28(void);
extern void isr_29(void);
extern void isr_30(void);
extern void isr_31(void);
extern void isr_32(void);
extern void isr_33(void);
extern void isr_34(void);
extern void isr_35(void);
extern void isr_36(void);
extern void isr_37(void);
extern void isr_38(void);
extern void isr_39(void);
extern void isr_40(void);
extern void isr_41(void);
extern void isr_42(void);
extern void isr_43(void);
extern void isr_44(void);
extern void isr_45(void);
extern void isr_46(void);
extern void isr_47(void);

/* Interrupt table. */
extern unsigned int isr_vector[INTERRUPTS];

#endif // KERNEL_ISRDEF_H_