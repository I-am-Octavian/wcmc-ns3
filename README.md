# NS-3 LTE Scheduling & Machine Learning Assignment

## 1. Project Overview
The objective of this assignment is to modify and evaluate LTE Scheduler algorithms in the NS-3 network simulator. The project involves two main components:
1.  **Part A:** A comparative performance analysis of four different LTE scheduling algorithms.
2.  **Part B:** The integration of a Machine Learning (ML) model to predict Modulation and Coding Scheme (MCS) values based on user location.

## 2. Simulation Topology & Parameters
The simulation setup consists of a 4-cell LTE network with the following configuration:

| Parameter | Value |
| :--- | :--- |
| **Topology** | 4 eNBs arranged in a grid |
| **Inter-eNB Distance** | 5 km |
| **Number of UEs** | 10 per eNB (40 total) |
| **UE Placement** | Random disc placement within 500m radius of eNB |
| **UE Mobility** | RandomWalk2d at speeds of 0 m/s and 10 m/s |
| **Traffic Type** | UDP (Downlink) |
| **Full Buffer Flow** | 1500 bytes every 1ms (12 Mbps) |
| **Non-Full Buffer Flow** | 1500 bytes every 10ms (1.2 Mbps) |
| **Tx Power** | 40 dBm (10 Watt) |
| **Bandwidth** | 50 RBs (DL and UL) |

## 3. Part A: Scheduler Comparison
The project evaluates the following scheduling algorithms available in the NS-3 LENA module:
* Proportional Fair (PF)
* Round Robin (RR)
* Max Throughput (MT)
* Priority Set Scheduler (PSS)

### Required Analysis
The schedulers are compared using the following metrics and visualizations:
* **Radio Environment Map (REM):** SINR visualization for the 4-cell topology.
* **System Throughput:** Average aggregate throughput vs. Speed (0, 10 m/s).
* **Fairness:** Throughput CDF plots for all schedulers.
* **Instantaneous Metrics:** SINR and Instantaneous throughput vs. Time for specific users .
* **Jain's Fairness Index (JFI):** Analysis of fairness variation across algorithms.

## 4. Part B: ML-Based MCS Prediction
This section involves training an ML model to predict network parameters based on user position.

### Workflow
1.  **Data Collection:** Run simulations (500s) to log UE coordinates ($x, y$) and their assigned MCS values.
2.  **Model Training:** Train a model (Split: 90% train, 10% test) to predict MCS using coordinates as input.
    * *Assigned Model:* Random Forest, LSTM, Bi-LSTM, RNN, or xGBoost (dependent on enrollment number) .
3.  **Prediction:** Implement a `GetMCS()` function to interface with the trained model during a 50s simulation, predicting MCS values in real-time and logging them against the original values.
