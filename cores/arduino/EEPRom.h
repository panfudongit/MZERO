
#pragma once
#define PAGE_SIZE 64          /* 64bytes */
#define NUMBER_OF_PAGES 4096  /*number of pages 4096 */

#define W_LOOP_COUNT 5
#define BOOTLOADER_SIZE 0x4000  /* bootloader size 16KB */
#define APP_SIZE        0x8000  /* app size 32KB        */
#define NVM_MEMORY        ((volatile uint16_t *)(FLASH_ADDR))

#define NVM_ERRORS_MASK (NVMCTRL_STATUS_PROGE | \
                           NVMCTRL_STATUS_LOCKE | \
                           NVMCTRL_STATUS_NVME)

#define TRUE 1
#define FALSE 0
class EEPROM
{
  public:
    EEPROM(unsigned int mode);
    unsigned short read(unsigned int addr);
    int write(unsigned int addr, unsigned short data);
    int loopwrite(unsigned int addr, unsigned short data);
    int nvm_erase(unsigned int addr);
    int nvm_execute_command(const unsigned int command, const unsigned int address, const unsigned int parameter);
    void end();

  private:
    int count;
};
