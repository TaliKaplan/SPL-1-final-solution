#include "../include/WareHouse.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "../include/Action.h"

// Constructor
WareHouse::WareHouse(const string &configFilePath): isOpen(false), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(),
completedOrders(), customers(), customerCounter(0), volunteerCounter(0), orderCounter(0)
{
    std::ifstream file(configFilePath);
    if (file.is_open()) {
        string line;
        while (std::getline(file,line)) {
            stringstream ss(line);
            vector<string> split;
            string s;
            while(getline(ss,s, ' ')) {
                split.push_back(s);
            }

            //checks if the line was empty
            if (split.empty())
                continue;
            // Customer
            if ((split[0] == "customer")) {
                addCustomer(split[1], split[2],stoi(split[3]), stoi(split[4])); // Name,Type,Distance,MaxOrders
            } 
            // Regular Collector
            if ((split[0] == "volunteer") && (split[2] == "collector")) {
                int id = newVolunteerId();
                addVolunteer(new CollectorVolunteer(id,split[1],stoi(split[3])));// Id,Name,Cooldown
                continue;
            }

            // Limited Collector
            if ((split[0] == "volunteer") && (split[2] == "limited_collector") ) {
                int id = newVolunteerId();
                addVolunteer(new LimitedCollectorVolunteer(id,split[1],stoi(split[3]),stoi(split[4])));// Id,Name,Cooldown,MaxOrders
                continue;
            }

            // Regular Driver
            if ((split[0] == "volunteer") && (split[2] == "driver") ) {
                int id = newVolunteerId();
                addVolunteer(new DriverVolunteer(id,split[1],stoi(split[3]),stoi(split[4])));// Id,Name,MaxDistance,DistancePerStep
                continue;
            }

            // Limited Driver
            if ((split[0] == "volunteer") && (split[2] == "limited_driver")) {
                int id = newVolunteerId();
                addVolunteer(new LimitedDriverVolunteer(id,split[1],stoi(split[3]),stoi(split[4]),stoi(split[5])));// Id,Name,MaxDistance,DistancePerStep,MaxOrders
                continue;
            }
        }
        file.close();
    }
}

// Copy constructor
WareHouse::WareHouse(const WareHouse &other):
isOpen(other.isOpen),actionsLog(),volunteers(),pendingOrders(),inProcessOrders(),
completedOrders(),customers(),customerCounter(other.customerCounter),volunteerCounter(other.volunteerCounter),orderCounter(other.orderCounter)
{
    for (Order *o:other.pendingOrders) {
        pendingOrders.push_back(o->clone());
    }

    for (Order *o:other.inProcessOrders) {
        inProcessOrders.push_back(o->clone());
    }

    for (Order *o:other.completedOrders) {
        completedOrders.push_back(o->clone());
    }

    for (Volunteer *v:other.volunteers) {
        volunteers.push_back(v->clone());
    }

    for (Customer *c:other.customers) {
        customers.push_back(c->clone());
    }

    for (BaseAction *a:other.actionsLog) {
        actionsLog.push_back(a->clone());
    }
}

// Move constructor
WareHouse::WareHouse(WareHouse &&other):
isOpen(other.isOpen),actionsLog(),volunteers(),pendingOrders(),inProcessOrders(),
completedOrders(),customers(),customerCounter(other.customerCounter),volunteerCounter(other.volunteerCounter),
orderCounter(other.orderCounter)
{
    for (Order *order:other.pendingOrders) {
        pendingOrders.push_back(order);
    }

    for (Order *order:other.inProcessOrders) {
        inProcessOrders.push_back(order);
    }

    for (Order *order:other.completedOrders) {
        completedOrders.push_back(order);
    }

    for (Volunteer *vol:other.volunteers) {
        volunteers.push_back(vol);
    }

    for (Customer *cust:other.customers) {
        customers.push_back(cust);
    }

    for (BaseAction *act:other.actionsLog) {
        actionsLog.push_back(act);
    }

    other.pendingOrders.clear();
    other.inProcessOrders.clear();
    other.completedOrders.clear();
    other.volunteers.clear();
    other.customers.clear();
    other.actionsLog.clear();
}

// Destructor
WareHouse::~WareHouse(){
    freeResources();
}

// copy Assignmet operator
WareHouse &WareHouse::operator=(const WareHouse &other) {
    if (this != &other) {
        freeResources();
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;

        for (Order *order:other.pendingOrders) {
            pendingOrders.push_back(order->clone());
        }

        for (Order *order:other.inProcessOrders) {
            inProcessOrders.push_back(order->clone());
        }

        for (Order *order:other.completedOrders) {
            completedOrders.push_back(order->clone());
        }

        for (Volunteer *vol:other.volunteers) {
            volunteers.push_back(vol->clone());
        }

        for (Customer *cust:other.customers) {
            customers.push_back(cust->clone());
        }

        for (BaseAction *act:other.actionsLog) {
            actionsLog.push_back(act->clone());
        }
    }

    return *this;
}

// Move assignmet 
WareHouse &WareHouse::operator=(WareHouse &&other) {
    if (this != &other) {
        freeResources();
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;

        for (Order *order:other.pendingOrders) {
            pendingOrders.push_back(order);
        }

        for (Order *order:other.inProcessOrders) {
            inProcessOrders.push_back(order);
        }

        for (Order *order:other.completedOrders) {
            completedOrders.push_back(order);
        }

        for (Volunteer *vol:other.volunteers) {
            volunteers.push_back(vol);
        }

        for (Customer *cust:other.customers) {
            customers.push_back(cust);
        }

        for (BaseAction *act:other.actionsLog) {
            actionsLog.push_back(act);
        }

        other.pendingOrders.clear();
        other.inProcessOrders.clear();
        other.completedOrders.clear();
        other.volunteers.clear();
        other.customers.clear();
        other.actionsLog.clear();
    }

    return *this;
}



void WareHouse::freeResources() {
    for (Order *o:pendingOrders) {
        delete o;
        o = nullptr;
    }
    pendingOrders.clear();

    for (Order *o:inProcessOrders) {
        delete o;
        o = nullptr;
    }
    inProcessOrders.clear();

    for (Order *o:completedOrders) {
        delete o;
        o = nullptr;
    }
    completedOrders.clear();

    for (Volunteer *v:volunteers) {
        delete v;
        v = nullptr;
    }
    volunteers.clear();

    for (Customer *c:customers) {
        delete c;
        c = nullptr;
    }
    customers.clear();

    for (BaseAction *a:actionsLog) {
        delete a;
        a = nullptr;
    }
    actionsLog.clear();
}

int WareHouse::newOrderId() {
    int res = orderCounter;
    orderCounter = orderCounter + 1;
    return res;
}

void WareHouse::addOrder(Order *order) {
    pendingOrders.push_back(order);
}

int WareHouse::newCustomerId() {
    int res = customerCounter;
    customerCounter = customerCounter + 1;
    return res;
}

void WareHouse::addCustomer(string name, string type, int dist, int maxOrders){
    if(type.compare("soldier")){
        SoldierCustomer* newCust = new SoldierCustomer(newCustomerId(), name, dist, maxOrders);
        customers.push_back(newCust); 
        return;
    }

    CivilianCustomer* newCust = new CivilianCustomer(newCustomerId(), name, dist, maxOrders);
    customers.push_back(newCust); 
}

int WareHouse::newVolunteerId() {
    int res = volunteerCounter;
    volunteerCounter = volunteerCounter + 1;
    return res;
}

void WareHouse::addVolunteer(Volunteer *volunteer) {
    volunteers.push_back(volunteer);
}

void WareHouse::addAction(BaseAction* action) {
    actionsLog.push_back(action);
}

const vector<BaseAction*> &WareHouse::getActions() const {
    return actionsLog;
}

// because customers aren't getting deleted during the simulation, every customerId below customerCounter belongs to an existing customer.
bool WareHouse::customerExists(int customerId) const {
    return (customerId < customerCounter);
}

// NOTE: this should only be called after verifying the customer exists.
Customer &WareHouse::getCustomer(int customerId) const {
    for (Customer *c:customers) {
        if (c->getId() == customerId) {
            return *c;
        }
    }

    
    // we shouldn't reach this part because we first check if it exists, we need this because the signature forces us to return a reference
    // and we need the compiler to be happy.
    static CivilianCustomer empty = CivilianCustomer(-1,"",0,0);
    return empty;
}

// TODO: this can be smarter with binary search, the volunteer vector is sorted by id.
bool WareHouse::volunteerExists(int volunteerId) const {
    for (Volunteer *v:volunteers) {
        if (v->getId() > volunteerId) {
            return false;
        }     
        if (v->getId() == volunteerId) {
            return true;
        }
    }
    return false;
}

// NOTE: this should only be called after verifying the volunteer exists.
Volunteer &WareHouse::getVolunteer(int volunteerId) const {
    for (Volunteer *v:volunteers) {
        if (v->getId() == volunteerId) {
            return *v;
        }
    }

    // we shouldn't reach this part because we first check if it exists, we need this because the signature forces us to return a reference
    // and we need the compiler to be happy.
    static CollectorVolunteer empty = CollectorVolunteer(-1,"",0);
    return empty;
}

// NOTE: we can't assume the vectors are sorted here because orders will jump between
// them unrelated to their id.
bool WareHouse::orderExists(int orderId) const {
    for (Order *o:pendingOrders) {
        if (o->getId() == orderId) {
            return true;
        }
    }

    for (Order *o:inProcessOrders) {
        if (o->getId() == orderId) {
            return true;
        }
    }

    for (Order *o:completedOrders) {
        if (o->getId() == orderId) {
            return true;
        }
    }

    return false;
}


// NOTE: this should only be called after verifying the order exists.
Order &WareHouse::getOrder(int orderId) const {
    for (Order *o:pendingOrders) {
        if (o->getId() == orderId) {
            return *o;
        }
    }

    for (Order *o:inProcessOrders) {
        if (o->getId() == orderId) {
            return *o;
        }
    }

    for (Order *o:completedOrders) {
        if (o->getId() == orderId) {
            return *o;
        }
    }

    // we shouldn't reach this part because we first check if it exists, we need this because the signature forces us to return a reference
    // and we need the compiler to be happy.
    static Order empty = Order(-1,-1,0);
    return empty;
}


void WareHouse::printOrders() const {

    for (Order *o:pendingOrders) {
        cout << "OrderID: " << o->getId() <<", CustomerID: " << o->getCustomerId() << ",OrderStatus: " << o->StatusToString() <<endl;
    }

    for (Order *o:inProcessOrders) {
        cout << "OrderID: " << o->getId() <<", CustomerID: " << o->getCustomerId() << ",OrderStatus: " << o->StatusToString() <<endl;
    }

    for (Order *o:completedOrders) {
        cout << "OrderID: " << o->getId() <<", CustomerID: " << o->getCustomerId() << ",OrderStatus: " << o->StatusToString() <<endl;
    }
}


vector<Order*> &WareHouse::getPendingOrders() {
    return pendingOrders;
}

vector<Order*> &WareHouse::getInProcessOrders() {
    return inProcessOrders;
}

vector<Order*> &WareHouse::getCompletedOrders() {
    return completedOrders;
}

vector<Volunteer*>& WareHouse::getVolunteers(){
    return volunteers;
}

void WareHouse::printActions() const{
    for (BaseAction* act : actionsLog){
        std::cout << act->toString() << std::endl;
    }
}

void WareHouse::start() {
    open();
    string userInput; // this holds the user's input for each iteration.
    do {
        std::getline(std::cin,userInput);
        stringstream ss(userInput);
        vector<string> split;

        string s;
        while(getline(ss,s, ' ')) {
            split.push_back(s);
        }

        if (split[0] == "step") {
            SimulateStep* step = new SimulateStep(stoi(split[1]));//sends number of steps
            step->act(*this);
            addAction(step);
            continue;
        }

        if (split[0] == "order") {
            AddOrder* order = new AddOrder(stoi(split[1]));//sends customer id
            order->act(*this);
            addAction(order);
            continue;
        }

        if (split[0] == "customer") {
            AddCustomer* cust = new AddCustomer(split[1], split[2], stoi(split[3]), stoi(split[4]));//(name, type, distance, max orders)
            cust->act(*this);
            addAction(cust);
            continue;
        }

        if (split[0] == "orderStatus") {
            PrintOrderStatus* orderStat = new PrintOrderStatus(stoi(split[1]));//(order id)
            orderStat->act(*this);
            addAction(orderStat);
            continue;
        }

        if (split[0] == "customerStatus") {
            PrintCustomerStatus* custStat = new PrintCustomerStatus(stoi(split[1]));//(customer id)
            custStat->act(*this);
            addAction(custStat);
            continue;
        }

        if (split[0] == "volunteerStatus") {
            PrintVolunteerStatus* volStat = new PrintVolunteerStatus(stoi(split[1]));//(volunteer id)
            volStat->act(*this);
            addAction(volStat);
            continue;
        }

        if (split[0] == "log") {
            PrintActionsLog* actLog = new PrintActionsLog();
            actLog->act(*this);
            addAction(actLog);
            continue;
        }

        if (split[0] == "close") {
            Close* close = new Close();
            close->act(*this);
            return;
        }

        if (split[0] == "backup") {
            BackupWareHouse* back = new BackupWareHouse();
            back->act(*this);
            continue;
        }

        if (split[0] == "restore") {
            RestoreWareHouse* restore = new RestoreWareHouse();
            restore->act(*this);
            addAction(restore);
            continue;
        }

    } while(userInput != "close");
}

void WareHouse::open() {
    isOpen = true;
    std::cout << "Warehouse is open!" << std::endl;
}

void WareHouse::close() {
    isOpen = false;
}
