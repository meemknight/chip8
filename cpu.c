#include "cpu.h"

void executeInstruction(unsigned char *c, regs_t *regs, uint16_t *stack, char *screen)
{
	uint16_t *instructions = (uint16_t*)c;
	int shouldIncreasePC = 1;

	if(regs->pc % 2 == 1)
	{
		yieldError(&regs, stack, "pc is odd.");
	}


	//uint16_t current = instructions[regs->pc / 2];
	//uint16_t instruction = *((uint16_t*)&c[regs->pc + 1]);
	uint8_t currentsmall = c[regs->pc];
	uint8_t currentsmall2 = c[regs->pc + 1];
	uint16_t instruction = (currentsmall << 8) + currentsmall2;
	//uint8_t h1 = ((uint8_t*)(&current))[0];
	//uint8_t h2 = ((uint8_t*)(&current))[1];

	//uint16_t *ptr = &current;

	//int a1 = (h1 & 0xF0) >> 4;
	//int b1 = h1 & 0x0F;
	//int c1 = (h2 & 0xF0) >> 4;
	//int d1 = h2 & 0x0F;


	switch (instruction & 0xF000)
	{

	case 0x0000:
	{
		uint16_t instruct = instruction;
		uint16_t nibble = instruction & 0xF000;
			switch (instruction & 0x0FFF)
			{
			case 0x00E0:
				//clears the screen
				for (int i = 0; i < 32 * 64; i++)
				{
					screen[i] = ' ';
				}
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
		regs->pc = instruction & 0x0FFF;
		shouldIncreasePC = 0;
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
		regs->pc = instruction & 0x0FFF;
		shouldIncreasePC = 0;
		break;
	}

	case 0x3000:
	{
		//Skip next instruction if Vx == kk.
		if (regs->v[(instruction & 0x0F00) >> 8] == instruction & 0x00FF)
		{
			regs->pc += 2;
		}
		break;
	}

	case 0x4000:
	{
		//3xkk Skip next instruction if Vx != kk.
		if(regs->v[(instruction & 0x0F00) >> 8] != instruction & 0x00FF)
		{
			regs->pc += 2;
		}
		break;
	}
	
	case 0x5000:
	{
		//Skip next instruction if Vx == Vy.
		if (regs->v[(instruction & 0x0F00) >> 8] == regs->v[(instruction & 0x00F0) >> 4])
		{
			regs->pc += 2;
		}
		break;
	}

	case 0x6000:
	{
		//Set Vx = kk.
		regs->v[(instruction & 0x0F00) >> 8] = instruction & 0x00FF;
		break;
	}

	case 0x7000:
	{
		//Set Vx = Vx + kk.
		regs->v[(instruction & 0x0F00) >> 8] += instruction & 0x00FF;
		break;
	}
	
	case 0x8000:
	{
		switch (instruction & 0x000F)
		{
		case 0x0000:
		{
			//8xy0
			//Set Vx = Vy.
		
			regs->v[(instruction & 0x0F00) >> 8] = regs->v[(instruction & 0x00F0) >> 4];

			break;
		}

		case 0x0001:
		{
			//8xy1
			//Set Vx = Vx OR Vy
			regs->v[(instruction & 0x0F00) >> 8] |= regs->v[(instruction & 0x00F0) >> 4];
			break;
		}

		case 0x0002:
		{
			//8xy2 - AND Vx, Vy
			//Set Vx = Vx AND Vy.
			regs->v[(instruction & 0x0F00) >> 8] &= regs->v[(instruction & 0x00F0) >> 4];
			break;
		}

		case 0x0003:
		{
			//8xy3 - XOR Vx, Vy
			//Set Vx = Vx XOR Vy.
			regs->v[(instruction & 0x0F00) >> 8] ^= regs->v[(instruction & 0x00F0) >> 4];
			break;
		}

		case 0x0004:
		{
			//8xy4 - ADD Vx, Vy
			//Set Vx = Vx + Vy, set VF = carry.
			if((int)regs->v[(instruction & 0x0F00) >> 8] + regs->v[(instruction & 0x00F0) >> 4] > 0xFF)
			{
				regs->vf = 1;
			}else
			{
				regs->vf = 0;
			}
			regs->v[(instruction & 0x0F00) >> 8] += regs->v[(instruction & 0x00F0) >> 4];
			break;
		}

		case 0x0005:
		{
			//8xy5 - SUB Vx, Vy
			//Set Vx = Vx - Vy, set VF = NOT borrow.
			if(regs->v[(instruction & 0x0F00) >> 8] > regs->v[(instruction & 0x00F0) >> 4])
			{
				regs->vf = 1;
			}else
			{
				regs->vf = 0;
			}
			regs->v[(instruction & 0x0F00) >> 8] -= regs->v[(instruction & 0x00F0) >> 4];

			break;
		}

		case 0x0006:
		{
			//8xy6 - SHR Vx {, Vy}
			//Set Vx = Vx SHR 1.
			if(regs->v[(instruction & 0x0F00) >> 8]%2 == 1)
			{
				regs->vf = 1;
			}else
			{
				regs->vf = 0;
			}

			regs->v[(instruction & 0x0F00) >> 8] >>= 1;

			break;
		}

		case 0x0007:
		{
			//8xy7 - SUBN Vx, Vy
			//Set Vx = Vy - Vx, set VF = NOT borrow.
			if (regs->v[(instruction & 0x00F0) >> 4] > regs->v[(instruction & 0x0F00) >> 8])
			{
				regs->vf = 1;
			}
			else
			{
				regs->vf = 0;
			}
			regs->v[(instruction & 0x0F00) >> 8] = regs->v[(instruction & 0x00F0) >> 4] - regs->v[(instruction & 0x0F00) >> 8];

			break;
		}

		case 0x000E:
		{
			//8xyE - SHL Vx {, Vy}
			//Set Vx = Vx SHL 1.

			if(regs->v[(instruction & 0x0F00) >> 8] & 0b1000'0000)
			{
				regs->vf = 1;
			}else
			{
				regs->vf = 0;
			}

			regs->v[(instruction & 0x0F00) >> 8] <<= 1;

			break;
		}
		default:

			yieldError(regs, stack, "illegal instruction 0x8...");
			break;

		}
		break;
	}

	case 0x9000:
		//Skip next instruction if Vx != Vy.
		
		if(regs->v[(instruction & 0x0F00) >> 8] != regs->v[(instruction & 0x00F0) >> 4])
		{
			regs->pc += 2;
		}
		
		break;

	case 0xA000:
		//Annn - LD I, addr
		//Set I = nnn.
		regs->i = instruction & 0x0FFF;
		break;

	case 0xB000:
		//Bnnn - JP V0, addr
		//Jump to location nnn + V0.
		regs->pc = (instruction & 0x0FFF) + regs->v0;
		shouldIncreasePC = 0;
		break;

	case 0xC000:
	{
		//Cxkk - RND Vx, byte
		//Set Vx = random byte AND kk.
		uint8_t r = rand() % 256;
		r &= (instruction & 0x00FF);

		regs->v[(instruction & 0x0F00) >> 8] = r;

		break;
	}
		
	case 0xD000:
	{
		//Dxyn - DRW Vx, Vy, nibble
		//Display n - byte sprite starting at memory location I at(Vx, Vy), set VF = collision.
		
		uint8_t x = (instruction & 0x0F00) >> 8;
		uint8_t y = (instruction & 0x00F0) >> 4;
		uint8_t n = (instruction & 0x000F);

		regs->vf = 0;

		for(int j=0; j<n; j++)
		{
			uint8_t line = c[regs->i + j];
			if(line == 0 )
			{
				continue;
			}

			for(int i = 7; i>=0; i--)
			{
				uint8_t pixel = line % 2;
				line = line >> 1;

				if(pixel)
				{
					if(getPixel(i + regs->v[x], j + regs->v[y], screen) == BLOCK_CHARACTER)
					{
						*getPixel(i + regs->v[x], j + regs->v[y], screen) = ' ';
						regs->vf = 1;
					}else
					{
						*getPixel(i + regs->v[x], j + regs->v[y], screen) = BLOCK_CHARACTER;
					}
				}

			}
		
		}

		break;
	}

	case 0xE000:
	{
		switch (instruction & 0x00FF)
		{
		case 0x009E:
			//Ex9E - SKP Vx
			//Skip next instruction if key with the value of Vx is pressed.

			if(isButtonPressed((instruction & 0x0F00) >> 8))
			{
				regs->pc += 2;
			}
			break;

		case 0x00A1:
			//ExA1 - SKNP Vx
			//Skip next instruction if key with the value of Vx is not pressed.
			
			if (!isButtonPressed((instruction & 0x0F00) >> 8))
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
		switch (instruction & 0x00FF)
		{
		case 0x0007:
			//Fx07 - LD Vx, DT
			//Set Vx = delay timer value.
			regs->v[(instruction & 0x0F00) >> 8] = regs->dt;
			break;

		case 0x000A:
		{
			//Fx0A - LD Vx, K
			//Wait for a key press, store the value of the key in Vx.
			uint8_t key = 255;
			shouldIncreasePC = 0;
				for(int i=0; i<16; i++)
				{
					if (isButtonPressed(i)) 
					{
						key = i;
						break;
					};
				}

				if(key != 255)
				{
					regs->v[(instruction & 0x0F00) >> 8] = key;
					shouldIncreasePC = 1;
				}

			break;
		}
		case 0x0015:
			//Fx15 - LD DT, Vx
			//Set delay timer = Vx.
			regs->dt = regs->v[(instruction & 0x0F00) >> 8];
			break;

		case 0x0018:
			//Fx18 - LD ST, Vx
			//Set sound timer = Vx.
			regs->st = regs->v[(instruction & 0x0F00) >> 8];
			break;

		case 0x001E:
			//Fx1E - ADD I, Vx
			//Set I = I + Vx.
			regs->i += regs->v[(instruction & 0x0F00) >> 8];
			
			break;

		case 0x0029:
		{	//Fx29 - LD F, Vx
			//Set I = location of sprite for digit Vx.

			uint8_t x = (instruction & 0x0F00) >> 8;
			if(regs->v[x] > 15)
			{
				yieldError(regs, stack, "illegal input for instrution Fx29");
			}
			regs->i = regs->v[x] * 5;

			break;
		}
		case 0x0033:
		{	//Fx33 - LD B, Vx
			//Store BCD representation of Vx in memory locations I, I + 1, and I + 2.
			int val = regs->v[(instruction & 0x0F00) >> 8];
			int ones = val % 10;
			val /= 10;
			int tens = val % 10;
			val /= 10;
			int hundreds = val % 10;
			c[regs->i] = hundreds;
			c[regs->i + 1] = tens;
			c[regs->i + 2] = ones;

			break;
		}
		case 0x0055:
		{
		//Fx55 - LD [I], Vx
		//Store registers V0 through Vx in memory starting at location I.
			
			for(int i=0; i < regs->v[(instruction & 0x0F00) >> 8]; i++)
			{
				c[regs->i + i] = regs->v[i];
			}			
			break;
		}
		case 0x0065:
		{
			//Fx65 - LD Vx, [I]
			//Read registers V0 through Vx from memory starting at location I.
			for (int i = 0; i < regs->v[(instruction & 0x0F00) >> 8]; i++)
			{
				regs->v[i] = c[regs->i + i];
			}
			break;
		}
		default:
			yieldError(regs, stack, "illegal instruction 0xF...");

			break;
		}
		//0xF...
		break;
	}

	default:
		yieldError(regs, stack, "illegal instruction ????");
		break;
	}

	if(shouldIncreasePC)
	{
		regs->pc += 2;
	}

	regs->cpuCount++;
	//printf("%d\n", regs->cpuCount);

}
