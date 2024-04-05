<!-- Please do not change this html logo with link -->

<a href="https://www.microchip.com" rel="nofollow"><img src="images/microchip.png" alt="MCHP" width="300"/></a>

# Bi-Phase Decoder — Use Case for CLB Using the PIC16F13145 Microcontroller with MCC Melody

The repository contains the Bi-Phase Decoder, an MPLAB® X project, using Core Independent Peripherals (CIPs) by following the interaction between Custom Logic Block (CLB), Serial Peripheral Interface (SPI) and Universal Asynchronous Receiver-Transmitter (UART) peripherals.

The CLB peripheral is a collection of logic elements that can be programmed to perform a wide variety of digital logic functions. The logic function may be completely combinatorial, sequential or a combination of the two, enabling users to incorporate hardware-based custom logic into their applications.

The Bi-Phase Mark Code (BMC) combines both data and clock in a single signal. One clock cycle is a BMC bit period. A transition always occurs at the beginning of each bit period. A logic `1` is represented by a transition (rising or falling edge) in the middle of the bit period and a logic `0` is represented by no transition in the middle of the period. A BMC encoder accepts a data signal and clock signal as inputs and produces a single BMC-encoded output. A BMC decoder accepts a BMC-encoded signal as the input and produces two outputs: data and clock. BMC is used in standards such as the USB 3.1 Power Delivery Specification CC signaling, AES3 digital audio or S/PDIF audio. The figure below presents an example:

<br><img src="images/biphase_signal.png" width="600">

## Related Documentation

More details and code examples on the PIC16F13145 can be found at the following links:

- [PIC16F13145 Product Page](https://www.microchip.com/en-us/product/PIC16F13145?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_PIC16F13145&utm_content=pic16f13145-biphase-decoder-mplab-mcc&utm_bu=MCU08)
- [PIC16F13145 Code Examples on Discover](https://mplab-discover.microchip.com/v2?dsl=PIC16F13145)
- [PIC16F13145 Code Examples on GitHub](https://github.com/microchip-pic-avr-examples/?q=PIC16F13145)
- [Bi-Phase Encoder — Use Case for CLB Using the PIC16F13145 Microcontroller with MCC Melody](https://github.com/microchip-pic-avr-examples/pic16f13145-biphase-encoder-mplab-mcc)
- [Bi-Phase Encoder and Decoder - Use Cases for CIPs Using the AVR128DA48 Microcontroller with MCC Melody](https://github.com/microchip-pic-avr-examples/avr128da48-cnano-biphase-mplab-mcc)

## Software Used

- [MPLAB X IDE v6.20 or newer](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_PIC16F13145&utm_content=pic16f13145-biphase-decoder-mplab-mcc&utm_bu=MCU08)
- [MPLAB® XC8 v2.46 or newer](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_PIC16F13145&utm_content=pic16f13145-biphase-decoder-mplab-mcc&utm_bu=MCU08)
- [PIC16F1xxxx_DFP v1.24.387 or newer](https://packs.download.microchip.com/)

## Hardware Used

- The [PIC16F13145 Curiosity Nano Development board](https://www.microchip.com/en-us/development-tool/EV06M52A?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_PIC16F13145&utm_content=pic16f13145-biphase-decoder-mplab-mcc&utm_bu=MCU08) is used as a test platform:
  <br><img src="images/pic16f13145-cnano.png" width="600">

## Operation

To program the Curiosity Nano board with this MPLAB X project, follow the steps provided in the [How to Program the Curiosity Nano Board](#how-to-program-the-curiosity-nano-board) chapter.<br><br>

## Concept

The encoded data is received through a single data wire. The Non-Return-to-Zero (NRZ) signal and clock signal are recovered using the circuit composed of the logic elements contained within the CLB peripheral described in the figure below. The resulting signals are routed to the SPI peripheral which reads and stores the data into the internal buffer called `FrameBuffer`, then the decoded data is transmitted further via the UART serial communication, when the encoded message is fully received.

<img src="images/clb_decoder_circuit.png" width="1000">

The Bi-Phase encoded signal is received through a single wire, while the decoding circuit is implemented using the CLB peripheral. The decoding circuit outputs the recovered NRZ data and a synchronized clock signal that are routed to the SPI peripheral configured in Client mode. The SPI peripheral will trigger the CLB circuit whenever a new byte is received, and the current byte will be stored in an internal buffer. The data is further transmitted via serial communication (UART). A time-out mechanism is implemented using the stop timer inside the CLB circuit, when no activity occurs on the Bi-Phase data line for the duration of six bytes.

An edge detector is implemented using an OR gate, as well as two DQ latches with Enable and Reset pins for data validation. When any edge is detected on the encoded input message, the first latch (left side of the picture) resets and outputs a `0` logic, the signal needed for the client select pin of the SPI peripheral. The hardware counter module, `pic16f131_counter`, is counting up to eight clock cycles for each bit of the encoded message and it is reset when an edge and the validation of the second latch (right side of the picture) are met into the AND gate. On the last counted bit, the counter is stopped, then the `StopCounter` sub-sheet, that is an implementation of a time-out counter inside the CLB, will count up to eight to detect if the frame is over, or it is reset if any other edge of the input is detected. When the hardware counter is on, the fifth output, that represents the 6/8 of the period, will enable the latches to output the decoded message for the SPI. The SPI clock is also recovered using the hardware counter, furthermore used to add a frequency tolerance due to possibility of the different operating frequency of the encoder/decoder. The fifth output of the hardware counter is used to verify the encoded bit of the message into the second half of it, not to determine if any other edge is detected in the middle of the transmission. The frequency of the CLB is given by the TMR2 Postscaler option, which means that the bit rate frequency of the decoder can be changed depending of the frequency of the encoded message.

## Setup

The following peripheral and clock configurations are set up using the MPLAB Code Configurator (MCC) Melody for the PIC16F13145:

1. Configuration Bits:

   - CONFIG1:
     - External Oscillator mode selection bits: Oscillator not enabled
     - Power-up default value for COSC bits: HFINTOSC (1MHz)
   - CONFIG2:
     - Brown-out reset enable bits: Brown-out reset disabled
   - CONFIG3:
     - WDT operating mode: WDT Disabled, SEN is ignored
       <br><img src="images/mcc_config_bits.png" width="400">

2. Clock Control:

   - Clock Source: HFINTOSC
   - HF Internal Clock: 32_MHz
   - Clock Divider: 1
     <br><img src="images/mcc_clock_control.png" width="400">

3. MSSP and SPI:

   - Serial Protocol: SPI
     - Mode: Client
     - SPI Mode: SPI Mode 3
     - Config Name: CLIENT_CONFIG
     - Interrupt Driven: Disabled
       <br><img src="images/mcc_spi.png" width="400">

4. CLB:

   - Enable CLB: Enabled
   - Clock Selection: TMR2_PostScaler
   - Clock Divider: Divide clock source by 1
     <br><img src="images/mcc_clb.png" width="400">

5. CRC:

   - Auto-configured by CLB

6. NVM:

   - Auto-configured by CLB

7. UART1:

   - Requested Baudrate: 57600
   - Data Size: 8
   - Receive Enable: Enabled
   - Serial Port Enable: Enabled
     <br><img src="images/mcc_uart.png" width="400"> <img src="images/mcc_eusart.png" width="400">

8. TMR2:

   - Enable Timer: Enabled
   - Control Mode: Roll over pulse
   - Start/Reset Option: Software control
   - Clock Source: HFINTOSC
   - Time Period (s): 0.000002 (2 μs)
     <br><img src="images/mcc_tmr_2.png" width="600">

9. Pin Grid View:
   - EUSART1 TX1: RC4
   - CLBPPSOUT0: RC3 (Data for SPI)
   - CLBPPSOUT1: RB4 (Clock for SPI)
   - CLBPPSOUT2: RB5 (Client Select for SPI)
   - CLBIN0PPS: RA2 (Encoded data from the encoder)
   - MSSP1 SCK: RB4
   - MSSP1 SDI: RC3
   - MSSP1 SS: RB5 (SS_SSP1 pin)
     <br><img src="images/mcc_pin_grid_view.png" width="600">

## Demo

In the demo, the `Microchip!` message was inserted by the user in the terminal and encoded on source. The signal received on the RA2 pin, Bi-Phase encoded signal, and the three outputs of the CLB are visualized using a logic analyzer.

<br><img src="images/decoder-demo1.png" width="1000">

To use the embedded decoder from the Logic software, the next analyzers settings must be set:

1. BMC settings:
   <br><img src="images/decoder-bmc-logic-settings.png" width="400">
2. SPI settings:
   <br><img src="images/decoder-spi-logic-settings.png" width="400">

Also, the `Microchip!` message was inserted by the user in the terminal. The output pin of the encoder platform (left side), BMC out (the output pin for the Bi-Phase encoded signal), is connected to the input pin of the decoder board and it is visualized using MPLAB Data Visualizer plug-in.

<br><img src="images/demo.gif" width="1000">

## Summary

This example demonstrates the capabilities of the CLB, a CIP, that can encode a message from the SPI and UART modules.

## How to Program the Curiosity Nano Board

This chapter demonstrates how to use the MPLAB X IDE to program a PIC® device with an `Example_Project.X`. This is applicable to other projects.

1.  Connect the board to the PC.

2.  Open the `Example_Project.X` project in MPLAB X IDE.

3.  Set the `Example_Project.X` project as main project.
    <br>Right click the project in the **Projects** tab and click **Set as Main Project**.
    <br><img src="images/Program_Set_as_Main_Project.png" width="600">

4.  Clean and build the `Example_Project.X` project.
    <br>Right click the `Example_Project.X` project and select **Clean and Build**.
    <br><img src="images/Program_Clean_and_Build.png" width="600">

5.  Select **PICxxxxx Curiosity Nano** in the Connected Hardware Tool section of the project settings:
    <br>Right click the project and click **Properties**.
    <br>Click the arrow under the Connected Hardware Tool.
    <br>Select **PICxxxxx Curiosity Nano** (click the **SN**), click **Apply** and then click **OK**:
    <br><img src="images/Program_Tool_Selection.png" width="600">

6.  Program the project to the board.
    <br>Right click the project and click **Make and Program Device**.
    <br><img src="images/Program_Make_and_Program_Device.png" width="600">

<br>

---

## Menu

- [Back to Top](#bi-phase-encoder--use-case-for-clb-using-the-pic16f13145-microcontroller-with-mcc-melody)
- [Back to Related Documentation](#related-documentation)
- [Back to Software Used](#software-used)
- [Back to Hardware Used](#hardware-used)
- [Back to Operation](#operation)
- [Back to Concept](#concept)
- [Back to Setup](#setup)
- [Back to Demo](#demo)
- [Back to Summary](#summary)
- [Back to How to Program the Curiosity Nano Board](#how-to-program-the-curiosity-nano-board)
