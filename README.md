<div id="top"></div>

<!-- PROJECT SHIELDS -->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about-the-project">About The Project</a></li>
    <li><a href="#donation">Donation</a></li>
    <li><a href="#components">Components</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#pictures">Pictures</a></li>
    <li><a href="#results">Results</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#refereces">Refereces</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->
# About The Project

### Welcome to Open-Watch ✋🏻😃 a wearable device for health monitoring and fitness tracking.

**Special thanks to [PCBWay](https://www.pcbway.com/) for sponsoring us on this open-source project and providing [these](#pictures) awesome 4-layer PCBs!** Open-Watch is an open-source smartwatch project developed for our B.Sc. final thesis. This smartwatch can measure the linear acceleration of your hand, rotational speed, heart rate, and SpO2 (oxygen saturation). All of these data will be processed by an STM32 processor with an ARM Cortex-M core. You can find these essential components [here](#components).

MPU6050 was used for IMU purposes. We use the Kalman filter to reduce the noise effect and extract clean data from the sensor (3-axis linear acceleration, 3-axis rotational velocity, and 3-axis rotation angle).

MAX30102 is the sensor used for health care purposes. We just read raw data and devolve the processes to the smartphone.

Open-watch will send these collected data to a smartphone via a Bluetooth module. On the smartphone side, an Android app reads the data from Bluetooth and processes them. For health care applications like heart rate and SpO2 analysis, we use ML-based algorithms to extract these parameters from raw data read from the pulse-oximeter sensor. In this section a real-time algorithm for analysis of photoplethysmography signal (PPG) for measurement of SpO2 will be implemented.

The final valuable data will be shown on a smartphone as well as the smartwatch screen.

<div align="center">
  <p> <img src="photos/Blocks.png?raw=true "Overview - Front"" width="800"> </p>  
</div>
<p align="right">(<a href="#top">back to top</a>)</p>

<!-- DONATION -->
# Donation

Do you want to support us in this project?

<p align="left">
  <a href="http://smotlaq.ir/LQgQF">
  <img src="https://raw.githubusercontent.com/SMotlaq/LoRa/master/bmc.png" width="200" alt="Buy me a Coffee"/>
  </a>
</p>

<p align="right">(<a href="#top">back to top</a>)</p>


# Components
 * 4-layer PCB: The PCBs of this project are provided by [PCBWay](https://www.pcbway.com), one of the best PCB factories in the world.
 * Main proccessor: [STM32F030C8 (ARM Cortex-M0)](https://www.javanelec.com/Shops/ProductDetail/24347)
 * Heartbeat and pulse oximeter sensor: [MAX30102](https://www.aliexpress.com/item/1005001863759524.html?spm=a2g0o.productlist.0.0.6b035ffbNirD5e&algo_pvid=579c2689-3578-4e82-948e-484fa9c52af9&algo_exp_id=579c2689-3578-4e82-948e-484fa9c52af9-0)
 * Motion sensor: [MPU6050](http://skytech.ir/product_details.aspx?ID_Parts=2830&MPU6050)
 * Display: [1.3" OLED 64*128 pixels](http://skytech.ir/product_details.aspx?ID_Parts=2006&1.3-inch-I2C-OLED-SSD1306-Module)
 * Vibration [motor](https://www.javanelec.com/Shops/ProductDetail/38512)
 * Bluetooth: [HC-05](https://www.javanelec.com/Shops/ProductDetail/30407)
 * Touch driver: [BS814-A](https://micromodern.ir/shop/touch-key-ic/bs814a-1-msop10/)
 * Battery: [3.7v Li-Po](https://micmodshop.ir/eshop/battery-products/batteries/battery-lithium-polymer-3-7v-400mah-custom-00/)
 * Battery charger: [TP4056](http://skytech.ir/product_details.aspx?ID_Parts=9929&TP4056%20SOP8)

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- ROADMAP -->
# Roadmap

- [x] Hardware design
  - [x] Microcontroller
  - [x] Vibration motor
  - [x] Bluetooth
  - [x] Buzzer
  - [x] Touch keys
  - [x] Charger
  - [X] PPG sensor
  - [X] MPU6050
- [X] PCB design
- [X] PCB Ordering
- [X] Body design
- [X] 3D print
- [x] Soldeing
- [x] Firmware programming
  - [x] RTC
  - [x] Alarm
  - [x] Bluetooth commands
  - [x] Vibration waves
  - [x] Buzzer
  - [x] OLED
    - [x] UI/UX
  - [x] Touch buttons
  - [x] Battery level indicator
  - [x] MPU6050
    - [x] Get raw data
    - [x] Kalman filter
    - [x] Movement detection
  - [x] PPG sensor
    - [x] Get raw data
    - [x] Transfer data using Bluetooth
- [x] Android programming
  - [x] Get blutooth data
  - [x] UI/UX
  - [x] Plot the raw PPG signals
  - [x] PPG data analysis
    - [x] Heartbeat
    - [x] SpO2
- [x] Assembling
- [x] Final test 😎

See the [open issues](https://github.com/SMotlaq/open-watch/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#top">back to top</a>)</p>


<!-- PARTS -->
# Pictures

**OpenWatch**
<p> <img src="photos/Pic45.png?raw=true "Fully assembled"" width="700"> </p> 

<!-- ![full](photos/Pic45.png?raw=true "Fully assembled") -->

**PCB Overview**
<p> <img src="photos/Pic38.jpg?raw=true "Overview - Front"" width="700"> </p> 
<p> <img src="photos/Pic39.jpg?raw=true "Overview - Back"" width="700"> </p> 

<!-- ![overview-front](photos/Pic38.jpg?raw=true "Overview - Front")
![overview-back](photos/Pic39.jpg?raw=true "Overview - Back") -->


**PCB - 2D** 
<p> <img src="photos/Pic33.jpg?raw=true "2D view of main PCB - both layers"" width="700"> </p> 

<!-- ![2D](photos/Pic33.jpg?raw=true "2D view of main PCB - both layers") -->


**PCB - 3D** 
<p> <img src="photos/Pic36.jpg?raw=true "3D view of main PCB - front"" width="700"> </p>  
<p> <img src="photos/Pic37.jpg?raw=true "3D view of main PCB - back"" width="700"> </p> 

<!-- ![3D-front](photos/Pic36.jpg?raw=true "3D view of main PCB - front")
![3D-back](photos/Pic37.jpg?raw=true "3D view of main PCB - back") -->


**PCB - Real**
<p> <img src="photos/Pic40.jpg?raw=true "Real PCB view - front"" width="700"> </p>  
<p> <img src="photos/Pic41.jpg?raw=true "Real PCB view - back"" width="700"> </p>
<p> <img src="photos/Pic42.jpg?raw=true "Real PCB view - soldered"" width="700"> </p>

<!-- ![real-front](photos/Pic32.jpg?raw=true "Real PCB view - front")
![real-back](photos/Pic31.jpg?raw=true "Real PCB view - back") -->

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- RESULTS -->
# Results

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- LICENSE -->
# License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- REFERENCES -->
# Refereces
**Dataset:**

MIMIC-III Database (https://mimic.mit.edu)

  [1] *Johnson, A., Pollard, T., & Mark, R. (2016). MIMIC-III Clinical Database (version 1.4). PhysioNet. https://doi.org/10.13026/C2XW26.*
  
  [2]	*Moody, B., Moody, G., Villarroel, M., Clifford, G., & Silva, I. (2020). MIMIC-III Waveform Database (version 1.0). PhysioNet. https://doi.org/10.13026/c2607m*
  
  [3]	*Kemp, J., Zhang, K., & Dai, A. (2020). MIMIC-III - SequenceExamples for TensorFlow modeling (version1.0.0). PhysioNet. https://doi.org/10.13026/n2v5-5b32.*
  
**Main references:**

 [1]	*Kiyasseh et al, CLOCS: Contrastive learning of cardiac signals across space, time, and patients, In Proc. ICML 2021*
 
 [2]	*Torres-Soto, J., Ashley, E.A. Multi-task deep learning for cardiac rhythm detection in wearable devices. npj Digit. Med. 3, 116 (2020). https://doi.org/10.1038/s41746-020-00320-4*
 
 [3]	*El Hajj C, Kyriacou P.A. Cuffless blood pressure estimation from PPG signals and its derivatives using deep learning models, Biomedical Signal Processing and Control. 70 (2021) https://doi.org/10.1016/j.bspc.2021.102984* 
 
 [4]	*Solà, J., & Delgado-Gonzalo, R. (2019). The Handbook of Cuffless Blood Pressure Monitoring: A Practical Guide for Clinicians, Researchers, and Engineers. The Handbook of Cuffless Blood Pressure Monitoring. https://doi.org/10.1007/978-3-030-24701-0*


<p align="right">(<a href="#top">back to top</a>)</p>


<!-- CONTACT -->
# Contact

Salman AmiMotlaq - [@SMotlaq](https://github.com/SMotlaq) - pilot.motlaq@gmail.com

Seyedmohammadsaleh Mirzatabatabaei - [@seyedsaleh](https://github.com/seyedsaleh) - seyedsaleh.edu@gmail.com

Amirhossein Aghajari - [@Aghajari](https://github.com/Aghajari) - amirhossein.aghajari.82@gmail.com

Project Link: [https://github.com/SMotlaq/open-watch](https://github.com/SMotlaq/open-watch)

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/SMotlaq/open-watch.svg?style=for-the-badge
[contributors-url]: https://github.com/SMotlaq/open-watch/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/SMotlaq/open-watch.svg?style=for-the-badge
[forks-url]: https://github.com/SMotlaq/open-watch/network/members
[stars-shield]: https://img.shields.io/github/stars/SMotlaq/open-watch.svg?style=for-the-badge
[stars-url]: https://github.com/SMotlaq/open-watch/stargazers
[issues-shield]: https://img.shields.io/github/issues/SMotlaq/open-watch.svg?style=for-the-badge
[issues-url]: https://github.com/SMotlaq/open-watch/issues
[license-shield]: https://img.shields.io/github/license/SMotlaq/open-watch.svg?style=for-the-badge
[license-url]: https://github.com/SMotlaq/open-watch/blob/master/LICENSE.txt

