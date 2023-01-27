
.text
.set noreorder

.global tlbRefillHandler
.global tlbRefillEPC
.global tlbRefillBadVAddr

.ent tlbRefillHandler
tlbRefillHandler:
	mfc0 $k0, $8 # Fetch BadVAddr
	sync.p
	sw $k0, tlbRefillBadVAddr

	mfc0 $k0, $14 # Fetch EPC
	sync.p
	sw $k0, tlbRefillEPC

	addi $k0, $k0, 4 # Skip to the instruction after our bad store
	mtc0 $k0, $14    # Set EPC
	sync.p
	eret # Exit this handler
	nop
.end tlbRefillHandler
