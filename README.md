# Project DOR
  A digital overcurrent relay (DOR) is a power system protection device that
initiates the tripping of a circuit breaker in the event of a fault. A DOR is better
than an electromechanical relay because it is able to have a completely
customisable characteristic (i.e. a I t ~ tripping curve) and is able to be set
and interrogated by a Supervisory Control and Data Acquisition (SCADA)
system.
  Since large power systems utilise a 3-phase circuit, there are three currents
(labelled a I , b I and cI ) that require independent monitoring. The aim of this
project is to implement a 3-phase DOR that is able to be set and interrogated
remotely (via the PC and USB interface).

## Project Structure
  This project is entirely based on the development board TWR-K70F120M with K70 families 32bits Arm Cortex-M4 MCUs.
  It consists of several haradware abstraction layers (HAL) writen in C++ and RTOS from the tutor, Peter McLean.
   

## Authors
  Author: Chao Li

