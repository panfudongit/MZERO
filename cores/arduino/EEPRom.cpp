
#include "EEPRom.h"
#include "Arduino.h"
#include "wiring_private.h"

static inline bool nvm_is_ready(void)
{
  return NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY;
}

void nvm_sync(void)
{
  while (!nvm_is_ready()) {
	/* Force-wait for the buffer clear to complete */
  }

}


EEPROM::EEPROM(unsigned int mode)
{
  PM->APBBMASK.reg |= PM_APBBMASK_NVMCTRL;

  NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;

  if (!(NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY))
    return;

  NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_SLEEPPRM(0x0ul) |
                       ((false & 0x01) << NVMCTRL_CTRLB_MANW_Pos) |
                       NVMCTRL_CTRLB_RWS(NVMCTRL->CTRLB.bit.RWS) |
                       (false << NVMCTRL_CTRLB_CACHEDIS_Pos) |
                       NVMCTRL_CTRLB_READMODE(0x0ul);

  if (NVMCTRL->STATUS.reg & NVMCTRL_STATUS_SB)
    return;
}

// 32b erase
int EEPROM::nvm_erase(unsigned int addr)
{
  if(addr > (PAGE_SIZE * NUMBER_OF_PAGES))
    return FALSE;
  if (addr & (((8 << NVMCTRL->PARAM.bit.PSZ) * NVMCTRL_ROW_PAGES) - 1))
    return FALSE;
  if (!(NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY))
    return FALSE;

  NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;
  NVMCTRL->ADDR.reg  = (uintptr_t)&NVM_MEMORY[addr / 4];
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_ER | NVMCTRL_CTRLA_CMDEX_KEY;

  nvm_sync();
  if ((NVMCTRL->STATUS.reg & NVM_ERRORS_MASK) != 0)
    return FALSE;

  return TRUE;

}

// read 16b
unsigned short EEPROM::read(unsigned int addr)
{
  if (addr & ((8 << NVMCTRL->PARAM.bit.PSZ) - 1))
    return FALSE;

  if (!nvm_is_ready())
    return FALSE;

  NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;

  return NVM_MEMORY[addr / 2];
}

//write 16b
int EEPROM::write(unsigned int addr, unsigned short data)
{
  int i = 0;

  do
  {
	i = i + 1; 
  }
  while (!loopwrite(addr, data) & (i < W_LOOP_COUNT));
  
}

int EEPROM::loopwrite(unsigned int addr, unsigned short data)
{
	nvm_erase(addr);

  if (addr & ((8 << NVMCTRL->PARAM.bit.PSZ) - 1))
    return FALSE;
  if (!(NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY))
    return FALSE;
  if (!nvm_is_ready())
    return FALSE;

  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_PBC | NVMCTRL_CTRLA_CMDEX_KEY;
  nvm_sync();

  NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;

  NVM_MEMORY[addr / 2] = data;

  nvm_execute_command(NVMCTRL_CTRLA_CMD_WP, addr, 0);

  return TRUE;

}

int EEPROM::nvm_execute_command(const unsigned int command, const unsigned int address, const unsigned int parameter)

{
  uint32_t ctrlb_bak;

  /* Check that the address given is valid  */
  if ((address > (PAGE_SIZE * NUMBER_OF_PAGES))
    && !(address >= NVMCTRL_AUX0_ADDRESS && address <= NVMCTRL_AUX1_ADDRESS )){
    return FALSE;
  }

  /* Turn off cache before issuing flash commands */
  ctrlb_bak = NVMCTRL->CTRLB.reg;

  NVMCTRL->CTRLB.reg = ctrlb_bak | NVMCTRL_CTRLB_CACHEDIS;

  /* Clear error flags */
  NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;

  /* Check if the module is busy */
  if (!nvm_is_ready()) {
    /* Restore the setting */
    NVMCTRL->CTRLB.reg = ctrlb_bak;
    return FALSE;
  }

  switch (command) {

    /* Commands requiring address (protected) */
    case NVMCTRL_CTRLA_CMD_EAR:
    case NVMCTRL_CTRLA_CMD_WAP:

      /* Auxiliary space cannot be accessed if the security bit is set */
      if (NVMCTRL->STATUS.reg & NVMCTRL_STATUS_SB) {
        /* Restore the setting */
        NVMCTRL->CTRLB.reg = ctrlb_bak;
        return FALSE;
      }

      /* Set address, command will be issued elsewhere */
      NVMCTRL->ADDR.reg = (uintptr_t)&NVM_MEMORY[address / 4];
      break;

    /* Commands requiring address (unprotected) */
    case NVMCTRL_CTRLA_CMD_ER:
    case NVMCTRL_CTRLA_CMD_WP:
    case NVMCTRL_CTRLA_CMD_LR:
    case NVMCTRL_CTRLA_CMD_UR:

      /* Set address, command will be issued elsewhere */
      NVMCTRL->ADDR.reg = (uintptr_t)&NVM_MEMORY[address / 4];
      break;

    /* Commands not requiring address */
    case NVMCTRL_CTRLA_CMD_PBC:
    case NVMCTRL_CTRLA_CMD_SSB:
    case NVMCTRL_CTRLA_CMD_SPRM:
    case NVMCTRL_CTRLA_CMD_CPRM:
      break;

    default:
      /* Restore the setting */
      NVMCTRL->CTRLB.reg = ctrlb_bak;
      return FALSE;
  }

  NVMCTRL->CTRLA.reg = command | NVMCTRL_CTRLA_CMDEX_KEY;

  nvm_sync();

  NVMCTRL->CTRLB.reg = ctrlb_bak;

  return TRUE;
}
