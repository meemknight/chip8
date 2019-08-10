#include "cpu.h"

void executeInstruction(char *c, regs_t *regs, uint16_t *stack)
{
	uint8_t *instructions = (uint8_t*)c;

	switch (instructions[regs->pc] & 0xF000)
	{
	case 0x0000:
	{
		
			switch (instructions[regs->pc] & 0x0FFF)
			{
			case 0x00E0:
				//todo clears the screen
				break;

			case 0x00EE:
				//Return from a subroutine.
				//The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
				if(regs->sp <= 0)
				{
					yieldError(&regs, stack, "0x00EE: stack is 0 or less.");
				}

				regs->pc = stack[regs->sp];
				regs->sp--;

				break;
			default:
				yieldError(&regs, stack, "deprecated 0x0nnn command");
				//jumps at nnn (deprecated)
				break;
			}
		break;
	}
		 
	case 0x1000:
	{
		//jumps at nnn
		regs->pc = instructions[regs->pc] & 0x0FFF;
		break;
	}

	case 0x2000:
	{
		//calls subroutine
		regs->sp++;
		if(regs->sp >= STACK_SIZE)
		{
			yieldError(&regs, stack, "0x2000: stack overflow");
		}

		stack[regs->sp] = regs->pc;
		regs->pc = instructions[regs->pc] & 0x0FFF;
		break;
	}

	case 0x3000:
	{
		//Skip next instruction if Vx == kk.
		if (regs->v[(instructions[regs->pc] & 0x0F00) >> 8] == instructions[regs->pc] & 0x00FF)
		{
			regs->pc += 2;
		}
		break;
	}

	case 0x4000:
	{
		//3xkk Skip next instruction if Vx != kk.
		if(regs->v[(instructions[regs->pc] & 0x0F00) >> 8] != instructions[regs->pc] & 0x00FF)
		{
			regs->pc += 2;
		}
		break;
	}
	
	case 0x5000:
	{
		//Skip next instruction if Vx == Vy.
		if (regs->v[(instructions[regs->pc] & 0x0F00) >> 8] == regs->v[(instructions[regs->pc] & 0x00F0) >> 4])
		{
			regs->pc += 2;
		}
		break;
	}

	case 0x6000:
	{
		//Set Vx = kk.
		regs->v[(instructions[regs->pc] & 0x0F00) >> 8] = instructions[regs->pc] & 0x00FF;
		break;
	}

	case 0x7000:
	{
		//Set Vx = Vx + kk.
		regs->v[(instructions[regs->pc] & 0x0F00) >> 8] += instructions[regs->pc] & 0x00FF;
		break;
	}
	
	case 0x8000:
	{
		switch (instructions[regs->pc] & 0x000F)
		{
		case 0x0000:
		{
			//8xy0
			//Set Vx = Vy.
		
			regs->v[(instructions[regs->pc] & 0x0F00) >> 8] = regs->v[(instructions[regs->pc] & 0x00F0) >> 4];

			break;
		}

		case 0x0001:
		{
			//8xy1
			//Set Vx = Vx OR Vy
			regs->v[(instructions[regs->pc] & 0x0F00) >> 8] |= regs->v[(instructions[regs->pc] & 0x00F0) >> 4];
			break;
		}

		case 0x0002:
		{
			//8xy2 - AND Vx, Vy
			//Set Vx = Vx AND Vy.
			regs->v[(instructions[regs->pc] & 0x0F00) >> 8] &= regs->v[(instructions[regs->pc] & 0x00F0) >> 4];
			break;
		}

		case 0x0003:
		{
			//8xy3 - XOR Vx, Vy
			//Set Vx = Vx XOR Vy.
			regs->v[(instructions[regs->pc] & 0x0F00) >> 8] ^= regs->v[(instructions[regs->pc] & 0x00F0) >> 4];
			break;
		}

		case 0x0004:
		{
			//8xy4 - ADD Vx, Vy
			//Set Vx = Vx + Vy, set VF = carry.
			if((int)regs->v[(instructions[regs->pc] & 0x0F00) >> 8] + regs->v[(instructions[regs->pc] & 0x00F0) >> 4] > 0xFF)
			{
				regs->vf = 1;
			}else
			{
				regs->vf = 0;
			}
			regs->v[(instructions[regs->pc] & 0x0F00) >> 8] += regs->v[(instructions[regs->pc] & 0x00F0) >> 4];
			break;
		}

		case 0x0005:
		{
			//8xy5 - SUB Vx, Vy
			//Set Vx = Vx - Vy, set VF = NOT borrow.
			if(regs->v[(instructions[regs->pc] & 0x0F00) >> 8] > regs->v[(instructions[regs->pc] & 0x00F0) >> 4])
			{
				regs->vf = 1;
			}else
			{
				regs->vf = 0;
			}
			regs->v[(instructions[regs->pc] & 0x0F00) >> 8] -= regs->v[(instructions[regs->pc] & 0x00F0) >> 4];

			break;
		}

		case 0x0006:
		{
			//8xy6 - SHR Vx {, Vy}
			//Set Vx = Vx SHR 1.

			if(regs->v[(instructions[regs->pc] & 0x0F00) >> 8]%2 == 1)
			{
				regs->vf = 1;
			}else
			{
				regs->vf = 0;
			}

			regs->v[(instructions[regs->pc] & 0x0F00) >> 8] >= 1;

			break;
		}

		case 0x0007:
		{
			//8xy7 - SUBN Vx, Vy
			//Set Vx = Vy - Vx, set VF = NOT borrow.
			if (regs->v[(instructions[regs->pc] & 0x00F0) >> 4] > regs->v[(instructions[regs->pc] & 0x0F00) >> 8])
			{
				regs->vf = 1;
			}
			else
			{
				regs->vf = 0;
			}
			regs->v[(instructions[regs->pc] & 0x0F00) >> 8] = regs->v[(instructions[regs->pc] & 0x00F0) >> 4] - regs->v[(instructions[regs->pc] & 0x0F00) >> 8];

			break;
		}

		case 0x000E:
		{
			//8xyE - SHL Vx {, Vy}
			//Set Vx = Vx SHL 1.

			if(regs->v[(instructions[regs->pc] & 0x0F00) >> 8] & 0b1000'0000)
			{
				regs->vf = 1;
			}else
			{
				regs->vf = 0;
			}

			regs->v[(instructions[regs->pc] & 0x0F00) >> 8] <= 1;

			break;
		}
		default:

			yieldError(regs, stack, "illegal instruction 0x8...");
			break;

		}

	}

	case 0x9000:
		//Skip next instruction if Vx != Vy.
		
		if(regs->v[(instructions[regs->pc] & 0x0F00) >> 8] != regs->v[(instructions[regs->pc] & 0x00F0) >> 4])
		{
			regs->pc += 2;
		}
		
		break;

	case 0xA000:
		//Annn - LD I, addr
		//Set I = nnn.
		regs->i = instructions[regs->pc] & 0x0FFF;
		break;

	case 0xB000:
		//Jump to location nnn + V0.
		regs->pc = instructions[regs->pc] & 0x0FFF + regs->v0;
		break;

	case 0xC000:
	{
		//Set Vx = random byte AND kk.
		uint8_t r = rand() % 256;
		r &= instructions[regs->pc & 0x00FF];

		instructions[(regs->pc & 0x0F00) >> 8] = r;
		break;
	}
		
	case 0xD000:
		//todo draw
		break;

	case 0xE000:
	{
		switch (instructions[regs->pc] & 0x00FF)
		{
		case 0x009E:
			//Ex9E - SKP Vx
			//Skip next instruction if key with the value of Vx is pressed.

			if(isButtonPressed((instructions[regs->pc] & 0x0F00) >> 8))
			{
				regs->pc += 2;
			}
			break;

		case 0x00A1:
			//ExA1 - SKNP Vx
			//Skip next instruction if key with the value of Vx is not pressed.
			
			if (!isButtonPressed((instructions[regs->pc] & 0x0F00) >> 8))
			{
				regs->pc += 2;
			}
			break;

		default:
			yieldError(regs, stack, "illegal instruction 0xE...");
			break;
		}
		break;
	}

	case 0xF000:
	{
		switch (instructions[regs->pc] & 0x00FF)
		{
		case 0x0007:
			//Fx07 - LD Vx, DT
			//Set Vx = delay timer value.
			regs->v[(instructions[regs->pc] & 0x0F00) >> 8] = regs->dt;
			break;

		case 0x000A:
		{
			//Fx0A - LD Vx, K
			//Wait for a key press, store the value of the key in Vx.
			uint8_t key = 255;
			
				for(int i=0; i<16; i++)
				{
					if (isButtonPressed(i)) 
					{
						key = i;
						break;
					};
				}
			//todo pause execution

				if(key != 255)
				{
					regs->v[(instructions[regs->pc] & 0x0F00) >> 8] = key;
				}

			break;
		}
		case 0x0015:
			//Fx15 - LD DT, Vx
			//Set delay timer = Vx.
			regs->dt = regs->v[(instructions[regs->pc] & 0x0F00) >> 8];
			break;

		case 0x0018:
			//Fx18 - LD ST, Vx
			//Set sound timer = Vx.
			
			regs->st = regs->v[(instructions[regs->pc] & 0x0F00) >> 8];
			break;

		case 0x001E:
			//Fx1E - ADD I, Vx
			//Set I = I + Vx.
			regs->i += regs->v[(instructions[regs->pc] & 0x0F00) >> 8];
			
			break;

		case 0x0029:
			//todo
			break;

		case 0x0033:
		{	//Fx33 - LD B, Vx
			//Store BCD representation of Vx in memory locations I, I + 1, and I + 2.
			int val = regs->v[(instructions[regs->pc] & 0x0F00) >> 8];
			int ones = val %10;
			val /= 10;
			int hundreds = val %10;
			val /= 10;
			int tens = val %10;
			instructions[regs->i] = hundreds;
			instructions[regs->i + 1] = tens;
			instructions[regs->i + 2] = ones;
			break;
		}
		case 0x0055:
		{
		//Fx55 - LD [I], Vx
		//Store registers V0 through Vx in memory starting at location I.
			
			for(int i=0; i< regs->v[(instructions[regs->pc] & 0x0F00) >> 8]; i++)
			{
				stack[regs->i + i] = regs->v[i];
			}			
			break;
		}
		case 0x0065:
		{
			//Fx65 - LD Vx, [I]
			//Read registers V0 through Vx from memory starting at location I.
			for (int i = 0; i < regs->v[(instructions[regs->pc] & 0x0F00) >> 8]; i++)
			{
				regs->v[i] = stack[regs->i + i];
			}
			break;
		}
		default:
			yieldError(regs, stack, "illegal instruction 0xF...");

			break;
		}

		break;
	}

	default:
		break;
	}


}
