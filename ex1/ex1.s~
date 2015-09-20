.syntax unified

.include "efm32gg.s"

/////////////////////////////////////////////////////////////////////////////
//
// Exception vector table
// This table contains addresses for all exception handlers
//
/////////////////////////////////////////////////////////////////////////////

.section .vectors

.long   stack_top               /* Top of Stack                 */
.long   _reset                  /* Reset Handler                */
.long   dummy_handler           /* NMI Handler                  */
.long   dummy_handler           /* Hard Fault Handler           */
.long   dummy_handler           /* MPU Fault Handler            */
.long   dummy_handler           /* Bus Fault Handler            */
.long   dummy_handler           /* Usage Fault Handler          */
.long   dummy_handler           /* Reserved                     */
.long   dummy_handler           /* Reserved                     */
.long   dummy_handler           /* Reserved                     */
.long   dummy_handler           /* Reserved                     */
.long   dummy_handler           /* SVCall Handler               */
.long   dummy_handler           /* Debug Monitor Handler        */
.long   dummy_handler           /* Reserved                     */
.long   dummy_handler           /* PendSV Handler               */
.long   dummy_handler           /* SysTick Handler              */

/* External Interrupts */
.long   dummy_handler
.long   gpio_handler            /* GPIO even handler */
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   gpio_handler            /* GPIO odd handler */
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler

.section .text 

.globl  _reset
.type   _reset, %function
.thumb_func


_reset: 
	//Branch and link to different setups (to make program more readable)
	BL register_clock
	BL register_led_lights
	BL register_buttons
	BL energy_saving
	BL register_interrupts
	WFI

register_clock:
	LDR R0, =CMU_BASE			//Load CMU_BASE address into register R0
	LDR R1, [R0, #CMU_HFPERCLKEN0]		//Load value of CMU_HFPERCLKEN0 into register R1
	MOV R2, #1				//Put value 1 directly in register R2
	LSL R2, R2, #CMU_HFPERCLKEN0_GPIO	//Left shift 1 to the bit enabling CMU_HFPERCLKEN0_GPIO
	ORR R1, R1, R2				//Use OR to edit the values in CMU_HFPERCLKEN0 so that we dont tamper with other values than the one we want to change.
	STR R1, [R0, #CMU_HFPERCLKEN0]		//Store result in memory
	BX LR					//Branch back to link register (return)

register_led_lights:
	LDR R0, =GPIO_PA_BASE			//Load GPIO port A base address into register R0
	MOV R2, #0x2				//Put 2 directly in register R2
	STR R2, [R0, #GPIO_CTRL]		//Store 2 in memory at the address of GPIO CTRL
	MOV R2, #0x55555555			//Put hex value 55555555 directly in register R2
	STR R2, [R0, #GPIO_MODEH]		//Store R2 in memory at the address of GPIO MODE HIGH.
	BX LR					//Branch back to link register					

register_buttons:
	LDR R0, =GPIO_PC_BASE			//Load address of GPIO port C base into register R0
	MOV R2, #0x33333333			//Put hex value 55555555 directly in register R2
	STR R2, [R0, #GPIO_MODEL]		//Store R2 in memory at the address of GPIO MODE LOW.
	MOV R2, #0xff				//Put hex value FF directly in register R2
	STR R2, [R0, #GPIO_DOUT]		//Store R2 in memory at the address of GPIO DOUT
	BX LR					//Branch back to link register

energy_saving:
	//Set EM3 (energy mode 3, stop mode, will wake up fast on interrupt)
	LDR R0, =EMU_BASE			//Load address of EMU Base
	MOV R1, #0				//Put value 0 directly in R1	
	STR R1, [R0, #EMU_CTRL]			//Store 0 back to memory at EMU CTRL.	

	//Enable deep sleep
	LDR R0, =SCR 				//Load System control block
	MOV R1, #6				//Put 6 directly in R1
	STR R1, [R0]				//Store R1 in SCR to enable deep sleep mode.

	//Disable some RAM-blocks
	LDR R0, =EMU_BASE			//Load address of EMU Base
	MOV R1, #0b111				//Put binary 111 in R1
	STR R1, [R0, #EMU_MEMCTRL]		//Store R1 to EMU Memory control set powerdown bits for block 1,2,3.
	
	//Clock optimization.
	LDR R0, =CMU_BASE			//Load address of CMU Base
	MOV R1, #0				//Put 0 in R1
	STR R1, [R0, #CMU_LFRCOCTRL]		//Store 0 in CMU LFRCOCTRL (tuning)
	STR R1, [R0, #CMU_HFRCOCTRL]		//Store 0 in CMU HFRCOCTRL, (set band and tuning)

	//Other things that could be done:
	// - Prevent GPIO-leakage. All unconnected pins should have GPIO MODEH/MODEL set to 0.

	BX LR					//Branch back to link register

	

register_interrupts:
	LDR R0, =GPIO_BASE			//Load GPIO BASE into register R0
	MOV R1, #0x22222222			//Put hex value 22222222 directly in register R1
	STR R1, [R0, #GPIO_EXTIPSELL]		//Store R1 in GPIO EXTIPSELL
	MOV R1, #0xff				//Put hex value FF directly in register R1
	STR R1, [R0, #GPIO_EXTIFALL]		//Store R1 in GPIO EXTIFALL
	STR R1, [R0, #GPIO_EXTIRISE]		//Store R1 in EXTIRISE
	STR R1, [R0, #GPIO_IEN]			//Store R1 in IEN
	LDR R0, =ISER0				//Load ISER0 to R1
	LDR R1, =#0x802				//Store hex value 802 in R1
	STR R1, [R0]				//Store R1 to address in R0
	BX LR					//Branch back to link register

.thumb_func
gpio_handler:
	LDR R0, =GPIO_BASE			//Load address of GPIO BASE in R0
	LDR R1, [R0, #GPIO_IF]			//Load value of interrupt flag.
	STR R1, [R0, #GPIO_IFC]			//Clear interrupt flag

	LDR R0, =GPIO_PC_BASE			//Load address of GPIO port C base to R0
	LDR R1, [R0, #GPIO_DIN]			//Load address of GPIO DIN to R1
	LSL R1, R1, #8				//Leftshifting value of R1 from bit postition 0-7 to position 8-15 which is the corresponding leds in our case.
	LDR R0, =GPIO_PA_BASE			//Load address of GPIO port A base to R0
	STR R1, [R0, #GPIO_DOUT]		//Store led values in R1 to GPIO DOUT on port A.
	BX LR					//Branch back to link register
	

/////////////////////////////////////////////////////////////////////////////

.thumb_func
dummy_handler:  
	b .  // do nothing

