
um() {
        memory = SegMem_new()
        registers = instantiate_registers()

        do {
        word = SegMem_get_next_i(memory)

        decode_and_execute(word, memory, registers)}
        while (word != halt)
}




word = SegMem_fetch_next_i

opcode = decode(word, &regA, &regB, &regC, &loadValReg, &loadval_value);
/* do the thing with the values and register depending on op code */

execute(opcode, regA, regB, regC, loadValReg, loadval_value);

execute() {
        switch(opcode) {
        case add:
                registers[regA] = registers[regB] + registers[regC];
                break
        case multiply: 
                registers[regA] = registers[regB] * registers[regC];
                break;
        }
}


uint32_t regs[8] = CALLOC(8 * sizeof(uint32_t));
FREE(regs)











static inline decode_and_execute(word, registers, memory) {
        opcode = get_opcode(word);
        switch (opcode) {
                case load_val:
                        reg = get_reg_loadval() /* in decode module */
                        load_val(word, registers)
                        break
                case not_load_val:
                        regA = get_reg_a(word); /* in decode module */
                        regB = get_reg_b(word) /* in decode module */
                        regC = get_reg_c(word) /* in decode module */
                        not_load_val(opcode, regA, regB, regC)
                        break
                case not_implemented:
        }
                
                
}

not_load_val(opcode, regA, regB, regC) {
        
        
}



/* in module execute*/
#include decode

add()
        // Get which 3 registers to act on

multiply()
        // Get which 3 registers to act on

//* in module decode 
get regs
get a
module instructions()
add()
multiply()


module decode
