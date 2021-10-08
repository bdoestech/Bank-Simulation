# Bank-Simulation
This program implements an  event-driven simulation of a bank customer service. In this simulation, a priority queue was used  to maintain/organize the arrival and departure events of the customers, and a regular FIFO queue  was used to maintain the bank waiting line. Based on provided input data, this program prints the  simulation statistics and results to an output file. 


------>Summary of Approach


In this program, there were 5 classes created. This includes the BankCustomer, Event, AQueue, 
PQueue, and BankTellerService classes. 

BankCustomer Class: 
   The purpose of this class is to maintain the data of each customer in the bank simulation. To create this class, the attributes were declared (all public), including ID,         ArrivalTime, TransactionTime, and ServiceStartTime (all int values). 

Event Class: 
   The purpose of this class is to represent the simulation customers’ arrival and departure events. To create this class, the attributes were declared (all public), including     EventType, CustID, and ADTime (char, int, and float respectively). 
  
AQueue and PQueue Classes: 
   The AQueue and PQueue classes that were created in problem 1 were also included in this program. The only difference between this AQueue class and problem 1’s is the array       of queue items is now referred to as IDs and a new function was added called queueLength(), which returns the current number of items in the queue. The only difference           between this PQueue class and problem 1’s is that it is a queue of Event objects, with their ADTime attribute as the prioritizing factor. 

BankTellerService Class: 
The purpose of this class is to simulate the bank teller service. To begin, the private attributes of the class were created, beginning with three int values: TellersNum, BusyTellers, and CustomersNum. An ifstream and ofstream attributes were created for the input and output text files. An AQueue called WaitingQ and A PQueue called EventsQ were created to the size of global constants (100 items). An array of BankCustomers called Customers was created to hold the Customer objects in the simulation. The final private attribute was an int value to store the maximum length of the waiting queue. Next, the public components of the class were created: 

   First, the constructor of the class was created which initializes the class parameters. It assigns TellersNum to an input parameter, dynamically creates the Customers array,     and opens the input/output text files. 

   Next, the destructor of the class was created. This deletes the memory allocated to the Customers array and closes the text files. 

   Next, the readCustomersInfo() function was created which reads the customers’ arrival and transaction times from the input file. With each arrival data from the input, a         customer object was added to the Customers array and an arrival event was enqueued into EventsQ. This was done by using a while loop to go through the input file and enter       the arrival times and transaction lengths into two separate arrays. Next, a for loop was used to enter the data into the new Customers and arrival Events, updating the           Customers array and EventsQ. 

   Next, the serveCustomers() function was created. This function executes a while loop that continues as long as there are events left in EventsQ. Each event is served in         order of the ADTime and according to the event type. If the event is an arrival, the customer either receives service from an available teller and a departure event is           enqueued to EventsQ (if BusyTellers<TellersNum), or the customer’s ID is enqueued to WaitingQ (if BusyTellers==TellersNum). If the event is a departure, a customer who has       been in the WaitingQ is served (if it is not empty) or the BusyTellers value is decremented (if it is empty). For each arrival and departure event in this simulation, the       program prints a message including the event type and ADTime to the output text file. 
      o To each departure Event’s ADTime, 0.01 was added so that the arrival Events would be prioritized over departures. 

   Finally, the getStatistics() function was created. This function calculates statistics based on the simulation data, including the number of tellers, the number of customers     processed, the average waiting time, the maximum waiting time, and the maximum waiting queue length. 
  
  
  To test this program, input data was created in an external text file.
