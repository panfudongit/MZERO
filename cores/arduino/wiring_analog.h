/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * \brief SAMD products have only one reference for ADC
 */
typedef enum _eAnalogReference
{
  AR_DEFAULT,
  AR_INTERNAL,
  AR_EXTERNAL,
  AR_INTERNAL1V0,
  AR_INTERNAL1V65,
  AR_INTERNAL2V23
} eAnalogReference ;

#define PWM_PRESCALER_PRESCALER_DIV_0 0x0u  //48MHz / 1
#define PWM_PRESCALER_PRESCALER_DIV_1 0x1u  //48MHz / 2
#define PWM_PRESCALER_PRESCALER_DIV_2 0x2u  //48MHz / 4
#define PWM_PRESCALER_PRESCALER_DIV_3 0x3u  //48MHz / 8
#define PWM_PRESCALER_PRESCALER_DIV_4 0x4u  //48MHz / 16
#define PWM_PRESCALER_PRESCALER_DIV_5 0x5u  //48MHz / 32
#define PWM_PRESCALER_PRESCALER_DIV_6 0x6u  //48MHz / 64
#define PWM_PRESCALER_PRESCALER_DIV_7 0x7u  //48MHz / 128

/*
 * \brief Configures the reference voltage used for analog input (i.e. the value used as the top of the input range).
 * This function is kept only for compatibility with existing AVR based API.
 *
 * \param ulMmode Should be set to AR_DEFAULT.
 */
extern void analogReference( eAnalogReference ulMode ) ;

/*
 * \brief Writes an analog value (PWM wave) to a pin.
 *
 * \param ulPin
 * \param ulValue
 */
extern void analogWrite( uint32_t ulPin, uint32_t ulValue ) ;

/*
 * \brief Reads the value from the specified analog pin.
 *
 * \param ulPin
 *
 * \return Read value from selected pin, if no error.
 */
extern uint32_t analogRead( uint32_t ulPin ) ;

/*
 * \brief Writes an analog value (PWM div) to a clock div.
 * \param ulValue
 */
extern void analogWriteDiv(int res);
/*
 * \brief Set the resolution of analogRead return values. Default is 10 bits (range from 0 to 1023).
 *
 * \param res
 */
extern void analogReadResolution(int res);

/*
 * \brief Set the resolution of analogWrite parameters. Default is 8 bits (range from 0 to 255).
 *
 * \param res
 */
extern void analogWriteResolution(int res);

extern void analogOutputInit( void ) ;

#ifdef __cplusplus
}
#endif
