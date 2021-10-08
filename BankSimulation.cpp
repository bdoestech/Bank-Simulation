//Brendan Li | NUID: 001386816
//EECE2560 Homework #5, part 2

#include <iostream>
#include <fstream>
#include <stdexcept>
#define maxints 100
using namespace std;
//global variables for EventsQ and WaitingQ
int const waitingQCapacity=100;
int const eventsQCapacity=100;

//Bank Customer Class
class BankCustomer{
public:
    //customer data:
    int ID;
    int ArrivalTime;
    int TransactionLength;
    int ServiceStartTime;
    //customer waiting time = ServiceStartTime-ArrivalTime;
    //customer departure time = ServiceStartTime+TransactionLength
};

//Event class, represents the simulation customers' arrival and departure events
class Event{
public:
    char EventType; //'A'=Arrival and 'D'=Departure
    int CustID; //ID of the customer who created the event
    float ADTime; //Arrival or Departure time
};


//AQueue class, same as HW5 part 1 but has some slight differences
class AQueue {
private: //declaring private variables
    int QCapacity; // max size of the queue
    int* IDs; //pointer to the array holding the customer's IDs
    int front; //index to front of queue
    int back; //index to back of queue
    int count; //number of items currently in queue

public:
    //class constructor
    AQueue(int c){
        //set QCapacity to given c, and create dynamic array
        QCapacity=c;
        IDs = new int[QCapacity];
        //inialize other variable defaults
        front=0;
        count=0;
        back=QCapacity-1;
    }

    //class destructor, deletes dynamic items
    ~AQueue(){
        delete IDs;
    }

    //returns true if the queue is empty
    bool isEmpty() const{
        return count==0;//return whether or not count == 0
    }

    //adds newItem to the back of the queue
    bool enqueue(const int newItem){
        //result boolean
        bool result = false;
        //if count isn't at the limit
        if(count<QCapacity){
            //update back
            back = (back+1)%QCapacity;
            IDs[back]=newItem;//add the new item to the back
            //update the count and the result bool
            count++;
            result=true;
        }
        return result;//return
    }

    //removes the item at the front of the queue
    bool dequeue(){
        //result boolean
        bool result = false;
        //if the queue is empty
        if(!isEmpty()){
            //update front
            front = (front+1)%QCapacity;
            //update count and result bool
            count--;
            result=true;
        }
        return result;//return
    }

    //returns the value of the item at the front of the queue
    int peekFront() const{
        return IDs[front];
    }

    //returns the current number of items in the queue
    int queueLength(){
        return count;
    }
};


//Class PQueue, same as from HW5 part 1 except i has Event objects as the contents and the ADTime attribute
//is used to prioritize them
class PQueue {
private: //declaring private variables
    int QCapacity; // maz size of the queue
    Event* events; //pointer to the array holding the queue's events
    int count; //number of items currently in queue
    //heapRebuild, transform semiheap rooted at rootIndex into minheap
    void heapRebuild(int rootIndex){
        //if the item is a leaf, nothing needs to be done
        if(rootIndex<=(((count-1)/2))&&rootIndex>=0){
            //keep track of smaller child index
            int smallerCI=2*rootIndex+1;
            //if the root has a right child
            if((2*rootIndex+2)<count){
                //then if the right child is less than the smaller, update index
                if(events[2*rootIndex+2].ADTime<events[smallerCI].ADTime){smallerCI=2*rootIndex+2;}
            }
            //if the root is greater than the index of the smaller child
            if(events[rootIndex].ADTime>events[smallerCI].ADTime){
                //swap and recursive call
                swap(events[rootIndex], events[smallerCI]);
                heapRebuild(smallerCI);
            }
        }
    }

public:
    //class constructor
    PQueue(int c){
        //set capacity to c, create dynamic array and set default count
        QCapacity=c;
        events = new Event[QCapacity];
        count=0;
    }

    //class destructor
    ~PQueue(){
        delete events;//delete dynamic memory
    }

    //returns true if the queue is empty
    bool isEmpty() const{
        return count==0;//return whether or not count==0
    }

    //adds newItem to the back of the queue
    bool enqueue(const Event newItem){
        //if the count is at the limit, return false already
        if(count==QCapacity){return false;}
        //set next item to the newItem
        events[count]=newItem;
        //set the newItem's index
        int newDataIndex=count;
        //boolean for while loop
        bool inPlace=false;
        int parentIndex;//index of the parent
        //while the index isn't 0 and not found place yet
        while((newDataIndex>0) && !inPlace){
            //find the parent index
            parentIndex=(newDataIndex-1)/2;
            //if the parent index is less than new, you found the place
            if((events[newDataIndex].ADTime>events[parentIndex].ADTime)){
                inPlace=true;
            }
            else {
                swap(events[parentIndex], events[newDataIndex]);
                newDataIndex=parentIndex;
            }
        }
        //update count and return
        count++;
        return true;
    }

    //removes the item at the front of the queue
    bool dequeue(){
        //copy last element to the root
        events[0]=events[count-1];
        //update the count
        count--;
        //rebuild the semi heap
        heapRebuild(0);
        return true;
    }

    //returns the value of the item at the front of the queue
    Event peekFront() const{
        return events[0];
    }
};


class BankTellerService{
private:
    int TellersNum; //total tellers in bank
    int BusyTellers; //how many busy tellers
    int CustomersNum; //total customers in sim
    ifstream inf; //text input file, arrival and transaction times
    ofstream otf; //text output file, trace messages and final stats
    AQueue WaitingQ = AQueue(waitingQCapacity);//Array Queue
    PQueue EventsQ = PQueue(eventsQCapacity);//Priority Queue
    BankCustomer* Customers; //array of customer objects in the sim
    int waitingQMax = 0; //maximum length of the waiting queue

public:
    //constructor
    BankTellerService(int t, string infS, string otfS){
        //Set attributes
        CustomersNum=0;
        TellersNum=t;
        Customers = new BankCustomer[waitingQCapacity];
        //code for opening text file
        //open at file location
        otf.open("\\Users\\Brend\\OneDrive\\Desktop\\EECE2560\\output.txt");
        //if this fails display error message
        if(otf.fail()){
            cerr << "Error: Could not open output file\n";
            exit(1);
        }
        //open at file location
        inf.open("\\Users\\Brend\\OneDrive\\Desktop\\EECE2560\\input.txt");
        //if this fails display error message
        if(inf.fail()){
            cerr << "Error: Could not open output file\n";
            exit(1);
        }
        //activate the exception handling of inf stream
        inf.exceptions(ifstream::failbit | ifstream::badbit);
        //activate the exception handling of otf stream
        otf.exceptions(ofstream::failbit | ofstream::badbit);
    }

    //deconstructor
    ~BankTellerService(){
        //delete dynamic allocated mem
        delete Customers;
        //close the two files
        inf.close();
        otf.close();
    }

    //reads transaction/arrival times from input file
    void readCustomersInfo(){
        //count for while loop
        int count = 0;
        //x stores the read inputs, a and t keep index of arrays
        int x; int a=0; int t=0;
        //arrays to store arrival times and also transaction lengths, separately
        int arrivals[maxints];
        int trans[maxints];
        //while loop
        while (count < maxints) { //keep reading until reading maxints or
            //until a reading failure is caught.
            //read inf, enter into x
            try {
                inf >> x;
            }
            //Check for reading failure due to end of file or
            // due to reading a non‐integer value from the file.
            catch (std::ifstream::failure e) {
                break;
            }
            //determine if the read value is a transactionlength or arrival time
            //update indices
            if(count%2==0){arrivals[a]=x; a++;}
            else{trans[t]=x; t++;}
            //update counter
            count++;
        }
        //create a temp customer and event
        BankCustomer temp;
        Event add;
        for(int i=0; i<a; i++){
            //declare the new customer
            temp=BankCustomer();
            //set ID/Arrival/Transaction
            temp.ID=i;
            temp.ArrivalTime=arrivals[i];
            temp.TransactionLength=trans[i];
            //update number of customers and put in place temp
            CustomersNum++;
            Customers[i]=temp;
            //create the arrival event and enqueue it to EventsQ
            add.EventType='A'; add.ADTime=Customers[i].ArrivalTime;
            add.CustID=i;
            EventsQ.enqueue(add);
        }
    }
    //serving customers
    //executes loop that continues as long as there are events in EventsQ
    void serveCustomers() {
        //busy tellers and max wait begin at 0
        BusyTellers=0;
        waitingQMax=0;
        //int cID for keeping track of ID, declare event for departures
        int cID;
        Event dep;
        //floats for calculating departure/wait times
        float running;
        float departT;
        otf<<"Simulation Traces: "<<endl;
        //while the EventsQ isn't empty, keep looping
        while (!EventsQ.isEmpty()) {
            //if the peeked event is an arrival
            if(EventsQ.peekFront().EventType=='A'){
                //if there is an available teller
                if(BusyTellers<TellersNum){
                    //store ID, get the running time, increment busytellers
                    cID=EventsQ.peekFront().CustID;
                    running=EventsQ.peekFront().ADTime;
                    BusyTellers++;
                    //print text to output file
                    otf << "Processing an arrival event at time <-- " << running << endl;
                    //create the departure event with specific attributes
                    dep.EventType='D';
                    dep.ADTime=(Customers[cID].TransactionLength) + running+0.01;//since it's a departure, add 0.01
                    dep.CustID=cID;
                    //dequeue top event, and enqueue departure
                    EventsQ.dequeue();
                    EventsQ.enqueue(dep);
                    //enter service start time for customer
                    Customers[cID].ServiceStartTime=running;
                }
                else{
                    //get ID and run time
                    cID=EventsQ.peekFront().CustID;
                    running=EventsQ.peekFront().ADTime;
                    //dequeue, print to output, enqueue to WaitingQ
                    EventsQ.dequeue();
                    otf << "Processing an arrival event at time <-- " << running << endl;
                    WaitingQ.enqueue(cID);
                }
            }
            //if the event is a departure
            else{
                //get ID and departT
                cID=EventsQ.peekFront().CustID;
                departT=EventsQ.peekFront().ADTime;
                //dequeue and find run time
                EventsQ.dequeue();
                running=departT+Customers[cID].TransactionLength;
                //print the output message (type cast to int to avoid displaying decimal)
                otf << "Processing a departure event at time --> " << (int)departT << endl;
                //if the waiting isn't empty, process another departure
                if(!WaitingQ.isEmpty()){
                    //find max waiting time and store it
                    if(WaitingQ.queueLength()>waitingQMax){waitingQMax=WaitingQ.queueLength();}
                    //create a new departure event
                    dep.EventType='D';
                    dep.ADTime=departT+Customers[WaitingQ.peekFront()].TransactionLength+0.01;
                    dep.CustID=WaitingQ.peekFront();
                    //Set the Service start time
                    Customers[WaitingQ.peekFront()].ServiceStartTime=departT;
                    //enqueue new departure and dequeue the waitingQ
                    EventsQ.enqueue(dep);
                    WaitingQ.dequeue();
                }
                //else, decrement the busy tellers
                else{
                    BusyTellers--;
                }
            }
        }

    }

    //Calculating some of the statistics
    void getStatistics(){
        otf<<"\n\nFinal Statistics:"<<endl;
        //print number of tellers and CustomersNum in output file
        otf<<"Number of tellers = "<<TellersNum<<endl;
        otf<<"Total number of costumers processed: "<<CustomersNum<<endl;
        //temp int to store each waiting time
        int tempWait;
        //ints to store max and sums
        int max=0;
        float sum=0;
        //go through each customer
        for(int i=0; i<CustomersNum; i++){
            tempWait=Customers[i].ServiceStartTime-Customers[i].ArrivalTime;
            //find max wait
            if(tempWait>max){max=tempWait;}
            //add to sum for average calculation
            sum=sum+tempWait;
        }
        //print to output file the average, max wait, and max waiting queue length
        otf<<"Average waiting time = "<<sum/CustomersNum<<endl;
        otf<<"Maximum waiting time = "<<max<<endl;
        otf<<"Maximum waiting queue length = "<<waitingQMax<<endl;
    }

};

//Provided main function
int main() {
    int T;
    cout << "How many tellers in the Bank? "<<endl;
    cin >> T;
    BankTellerService myBank(T, "input.txt", "output.txt");
    myBank.readCustomersInfo();
    myBank.serveCustomers();
    myBank.getStatistics();
    return 0;
}
