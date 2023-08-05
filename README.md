# IoT - Irrigation System

Automated irrigation system that monitors soil moisture levels and controls the water flow to the plants accordingly. The system will be controlled through a web or mobile application, and it will use sensors to collect data and actuators to control water flow.

## Components

1. **Web/Mobile Application:** Allows the user to control the system and visualize the data collected by the microcontroller. Interacts with the API.

2. **API:** Allows communication between Web/Mobile Application and Microcontroller. Interacts with the Kafka Cluster to send and receive messages from the microcontroller.

3. **Kafka Cluster:** Allows communication between API and Microcontroller.

4. **Microcontoller:** Collects data from the sensors and controls the actuators. Interacts with the Kafka Cluster to send and receive messages from the API.

![ComponentsDiagram](/docs/static/components-diagram.png)


## Features and Requirements

### Web/Mobile Application

- [ ] User can register and login.
- [ ] User can see the current status of the system.
- [ ] User can see the current status of the sensors.
- [ ] User can see the current status of the actuators.
- [ ] User can see the history of the sensors.
- [ ] User can see the history of the actuators.
- [ ] User can change the status of the actuators.
- [ ] User can change the configuration of the system.

### API

- [ ] API can receive messages from the Kafka Cluster.
- [ ] API can send messages to the Kafka Cluster.
- [ ] API can receive messages from the Web/Mobile Application.
- [ ] API can send messages to the Web/Mobile Application.

### Kafka Cluster

- [ ] Kafka Cluster can receive messages from the API.
- [ ] Kafka Cluster can send messages to the API.
- [ ] Kafka Cluster can receive messages from the Microcontroller.
- [ ] Kafka Cluster can send messages to the Microcontroller.

### Microcontroller

- [ ] Microcontroller can receive messages from the Kafka Cluster.
- [ ] Microcontroller can send messages to the Kafka Cluster.
- [ ] Microcontroller can receive messages from the sensors.
- [ ] Microcontroller can send messages to the actuators.

## Architecture




