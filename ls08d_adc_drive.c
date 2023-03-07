/*-
 * Copyright (c) <year>, zhbi98, 3049824528@qq.com
 * All rights reserved.
 *
 * DISCLAIMED, IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ls08d_adc_drive.h"

LS08D_TypeDef adcLS08D = {
    ._adc0Phase = 0,
    ._adc1Phase = 0,
    ._speed = STATE_500MHZ;
};

void LS08D_TransmitDelay(unsigned int Nms)
{
    /* delay code */
}

void LS08D_RelayTransmit16Bits(unsigned char relayAddr, unsigned short relayData)
{
    /* Transmit code */
}

void LS08D_RelayTransmit32Bits(unsigned char relayAddr, unsigned short relayData)
{
    /* Transmit code */
}

/*************************************************************
 *    THE FOLLOWING NEW ADC LS08D DRIVER
 *************************************************************/
void LS08D_RegisterReset(unsigned char adcResetPinLevel, unsigned char adcPwr1PinLevel, unsigned char adcPwr2PinLevel)
{
    volatile unsigned char data = 0x00;

    data =  ((adcPwr2PinLevel  & 0x01) << 2) | 
            ((adcPwr1PinLevel  & 0x01) << 1) | 
            ((adcResetPinLevel & 0x01) << 0);

    LS08D_RelayTransmit32Bits(FpgaAddr.lsd_adc_reset, data);
    sleep_ms(10);
    LS08D_RelayTransmit32Bits(FpgaAddr.lsd_adc_reset, data);
}

void LS08D_FpgaPhaseReset()
{
    LS08D_TransmitDelay(50);
    LS08D_RelayTransmit16Bits(FpgaAddr.adc_clock_phase_start, 1);
    LS08D_TransmitDelay(5);
}

void LS08D_WriteRegister(unsigned char adcNum, unsigned char adcRegs, unsigned short regsValue)
{
    /**
     * adcNum:0 -> adc0 write register
     * adcNum:1 -> adc1 write register
     * Data all 4 bytes:
     * 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000
     */
    volatile unsigned int data 0x00;

    data = ((adcNum & 0x01) << 24) | ((adcRegs & 0xFF) << 16) | (regsValue & 0xFFFF);

    LS08D_TransmitDelay(20);
    LS08D_RelayTransmit32Bits(FpgaAddr.hmcad15xx, data);
    LS08D_TransmitDelay(20);
}

void LS08D_RegisterInit()
{
    /***********************************
     *    LS08D ADC RESET POWER DOWN
     ***********************************/
    LS08D_RegisterReset(0, 1, 1);
    LS08D_TransmitDelay(1);
    LS08D_RegisterReset(1, 0, 0);
    LS08D_TransmitDelay(10);
    LS08D_RegisterReset(0, 0, 0);
    LS08D_TransmitDelay(10);

    /***********************************
     *    LS08D ADC PREPARE INIT
     ***********************************/
    LS08D_WriteRegister(0, 0x2E, 0xFFFF);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(1, 0x2E, 0xFFFF);
    LS08D_TransmitDelay(5);

    /***********************************
     *    LS08D ADC 0 INIT
     ***********************************/
    LS08D_WriteRegister(0, 0x12, 0x4ADD);
    LS08D_TransmitDelay(5);

    switch (adcLS08D._speed) {
    case STATE_500MHZ:
        adcLS08D._delaultPhaseADC0 = 0xF13F;
        adcLS08D._delaultPhaseADC0 = (adcLS08D._delaultPhaseADC0 & 0xFF8F) | ((_phaseADC0 & 0x07) << 4) 
        LS08D_WriteRegister(0, 0x29, phaseADC0);
        LS08D_TransmitDelay(5);
        LS08D_WriteRegister(0, 0x2A, 0xCC26);
        LS08D_TransmitDelay(5);
        break;
    case STATE_1000MHZ:
        adcLS08D._delaultPhaseADC0 = 0xD10F;
        adcLS08D._delaultPhaseADC0 = (adcLS08D._delaultPhaseADC0 & 0xFF8F) | ((_phaseADC0 & 0x07) << 4) 
        LS08D_WriteRegister(0, 0x29, phaseADC0);
        LS08D_TransmitDelay(5);
        LS08D_WriteRegister(0, 0x2A, 0x3C26);
        LS08D_TransmitDelay(5);
        break;
    }


    // 500MHz:0xF13F, 1000MHz:0xD10F
    // LS08D_WriteRegister(0, 0x29, 0xF13F);
    phaseADC0 = (
        phaseADC0 & 0xFF8F
    ) | ((phaseUser0 & 0x07) << 4);
    LS08D_WriteRegister(0, 0x29, phaseADC0);
    LS08D_TransmitDelay(5);

    LS08D_WriteRegister(0, 0x30, 0x0007);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(0, 0x13, 0x8038);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(0, 0x08, 0xE600);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(0, 0x24, 0x0127);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(0, 0x2D, 0x0044);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(0, 0x15, 0x0103);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(0, 0x02, 0x0600);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(0, 0x05, 0x0600);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(0, 0x08, 0xE600);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(0, 0x0B, 0x0600);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(0, 0x0E, 0x0600);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(0, 0x0F, 0x4008);
    LS08D_TransmitDelay(5);

#if 0 // Not use
    LS08D_WriteRegister(0, 0x11, 0x200E);
    LS08D_TransmitDelay(5);
#endif

    /***********************************
     *    LS08D ADC 1 INIT
     ***********************************/
    LS08D_WriteRegister(1, 0x12, 0x4ADD);
    LS08D_TransmitDelay(5);

    switch (adcLS08D._speed) {
    case STATE_500MHZ:
        adcLS08D._delaultPhaseADC0 = 0xF13F;
        adcLS08D._delaultPhaseADC0 = (adcLS08D._delaultPhaseADC0 & 0xFFF1) | ((_phaseADC0 & 0x07) << 1) 
        LS08D_WriteRegister(1, 0x29, phaseADC0);
        LS08D_TransmitDelay(5);
        LS08D_WriteRegister(1, 0x2A, 0xCC26);
        LS08D_TransmitDelay(5);
        break;
    case STATE_1000MHZ:
        adcLS08D._delaultPhaseADC1 = 0xD10F;
        adcLS08D._delaultPhaseADC1 = (adcLS08D._delaultPhaseADC0 & 0xFFF1) | ((_phaseADC0 & 0x07) << 1) 
        LS08D_WriteRegister(1, 0x29, phaseADC0);
        LS08D_TransmitDelay(5);
        LS08D_WriteRegister(1, 0x2A, 0x3C26);
        LS08D_TransmitDelay(5);
        break;
    }

    LS08D_WriteRegister(1, 0x30, 0x0007);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(1, 0x13, 0x8038);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(1, 0x08, 0xE600);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(1, 0x24, 0x0127);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(1, 0x2D, 0x0044);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(1, 0x15, 0x0103);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(1, 0x02, 0x0600);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(1, 0x05, 0x0600);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(1, 0x08, 0xE600);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(1, 0x0B, 0x0600);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(1, 0x0E, 0x0600);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(1, 0x0F, 0x4008);

#if 0 // Not use
    LS08D_WriteRegister(1, 0x11, 0x200E);
    LS08D_TransmitDelay(5);
#endif

    LS08D_TransmitDelay(100);
}

void LS08D_RegisterDeinit()
{
    LS08D_RegisterReset(1, 0, 0);
    LS08D_TransmitDelay(10);
    LS08D_RegisterReset(0, 0, 0);
    LS08D_TransmitDelay(10);
    LS08D_RegisterReset(1, 0, 0);
    LS08D_TransmitDelay(10);
    LS08D_RegisterReset(0, 1, 1);
    LS08D_TransmitDelay(10);
}

static const unsigned short LSD08D_0X11Arr[] = {
    0x270E, 0x250E, 0x230E, 0x210E,
    0x2F0E, 0x272E, 0x2D0E, 0x252E,
    0x2B0E, 0x232E, 0x290E, 0x212E,
    0x2F2E, 0x274E, 0x2D2E, 0x254E,
    0x2B2E, 0x234E, 0x292E, 0x214E,
    0x2F4E, 0x276E, 0x2D4E, 0x256E,
    0x2B4E, 0x236E, 0x294E, 0x216E,
    0x2F6E, 0x278E, 0x2D6E, 0x258E,
    0x2B6E, 0x238E, 0x296E, 0x218E,
    0x2F8E, 0x27AE, 0x2D8E, 0x25AE,
    0x2B8E, 0x23AE, 0x298E, 0x21AE,
    0x2FAE, 0x27CE, 0x2DAE, 0x25CE,
    0x2BAE, 0x23CE, 0x29AE, 0x21CE,
    0x2FCE, 0x27EE, 0x2DCE, 0x25EE,
    0x2BCE, 0x23EE, 0x21EE, 0x29CE,
    0x260E, 0x2FEE, 0x240E, 0x2DEE,
    0x220E, 0x2BEE, 0x200E, 0x29EE,
    0x262E, 0x2E0E, 0x242E, 0x2C0E,
    0x222E, 0x2A0E, 0x202E, 0x280E,
    0x264E, 0x2E2E, 0x244E, 0x2C2E,
    0x224E, 0x2A2E, 0x204E, 0x282E,
    0x266E, 0x2E4E, 0x246E, 0x2C4E,
    0x226E, 0x2A4E, 0x206E, 0x284E,
    0x268E, 0x2E6E, 0x248E, 0x2C6E,
    0x228E, 0x2A6E, 0x208E, 0x286E,
    0x26AE, 0x2E8E, 0x24AE, 0x2C8E,
    0x22AE, 0x2A8E, 0x20AE, 0x26CE,
    0x288E, 0x2EAE, 0x24CE, 0x2CAE,
    0x22CE, 0x2AAE, 0x20CE, 0x26EE,
    0x28AE, 0x2ECE, 0x24EE, 0x2CCE,
    0x22EE, 0x2ACE, 0x20EE, 0x28CE,
    0x2EEE, 0x2CEE, 0x2AEE, 0x28EE,
};

static const unsigned short LS08D_GainArr[] = {
    0x638, 0x628, 0x618, 0x608,
    0x678, 0x639, 0x668, 0x629,
    0x658, 0x619, 0x648, 0x609,
    0x679, 0x63A, 0x669, 0x62A,
    0x659, 0x61A, 0x649, 0x60A,
    0x67A, 0x63B, 0x66A, 0x62B,
    0x65A, 0x61B, 0x64A, 0x60B,
    0x67B, 0x63C, 0x66B, 0x62C,
    0x65B, 0x61C, 0x64B, 0x60C,
    0x67C, 0x63D, 0x66C, 0x62D,
    0x65C, 0x61D, 0x64C, 0x60D,
    0x67D, 0x63E, 0x66D, 0x62E,
    0x65D, 0x61E, 0x64D, 0x60E,
    0x67E, 0x63F, 0x66E, 0x62F,
    0x65E, 0x61F, 0x60F, 0x64E,
    0x630, 0x67F, 0x620, 0x66F,
    0x610, 0x65F, 0x600, 0x64F,
    0x631, 0x670, 0x621, 0x660,
    0x611, 0x650, 0x601, 0x640,
    0x632, 0x671, 0x622, 0x661,
    0x612, 0x651, 0x602, 0x641,
    0x633, 0x672, 0x623, 0x662,
    0x613, 0x652, 0x603, 0x642,
    0x634, 0x673, 0x624, 0x663,
    0x614, 0x653, 0x604, 0x643,
    0x635, 0x674, 0x625, 0x664,
    0x615, 0x654, 0x605, 0x636,
    0x644, 0x675, 0x626, 0x665,
    0x616, 0x655, 0x606, 0x637,
    0x645, 0x676, 0x627, 0x666,
    0x617, 0x656, 0x607, 0x646,
    0x677, 0x667, 0x657, 0x647,
};

void LS08D_TwoADCGainFix(unsigned char adcNum, unsigned char gainIndex)
{
    if (gainIndex > LS08D_ARR_SIZE - 1)
        gainIndex = LS08D_ARR_SIZE - 1;

    LS08D_WriteRegister(adcNum, 0x02, LS08D_GainArr[gainIndex]);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(adcNum, 0x05, LS08D_GainArr[gainIndex]);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(adcNum, 0x08, LS08D_GainArr[gainIndex]);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(adcNum, 0x0B, LS08D_GainArr[gainIndex]);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(adcNum, 0x0E, LS08D_GainArr[gainIndex]);
    LS08D_TransmitDelay(5);
    LS08D_WriteRegister(adcNum, 0x11, LSD08D_0X11Arr[gainIndex]);
    LS08D_TransmitDelay(5);
}
