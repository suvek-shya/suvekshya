#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>

class Item {
public:
    std::string name;
    double price;
    int quantity;

    Item(const std::string& n, double p, int q) : name(n), price(p), quantity(q) {}
};

class User {
public:
    std::string username;
    std::string password;
    bool isOwner;
    std::vector<Item> cart;

    User(const std::string& u, const std::string& p, bool o) : username(u), password(p), isOwner(o) {}

    void addToCart(const Item& item, int quantity) {
        auto it = std::find_if(cart.begin(), cart.end(),
            [&item](const Item& cartItem) { return cartItem.name == item.name; });

        if (it != cart.end()) {
            it->quantity += quantity;
        } else {
            cart.push_back(Item(item.name, item.price, quantity));
        }
    }

    void viewCart() const {
        if (cart.empty()) {
            std::cout << "Your cart is empty." << std::endl;
            return;
        }

        std::cout << "\n+-------------------+------------+----------+------------+" << std::endl;
        std::cout << "| Item Name         | Price      | Quantity | Total      |" << std::endl;
        std::cout << "+-------------------+------------+----------+------------+" << std::endl;

        double total = 0;
        for (const auto& item : cart) {
            double itemTotal = item.price * item.quantity;
            total += itemTotal;
            std::cout << "| " << std::left << std::setw(17) << item.name.substr(0, 17) << " | "
                      << std::right << std::setw(10) << std::fixed << std::setprecision(2) << item.price << " | "
                      << std::setw(8) << item.quantity << " | "
                      << std::setw(10) << std::fixed << std::setprecision(2) << itemTotal << " |" << std::endl;
        }

        std::cout << "+-------------------+------------+----------+------------+" << std::endl;
        std::cout << "| Total                                     | " << std::setw(10) << std::fixed << std::setprecision(2) << total << " |" << std::endl;
        std::cout << "+-----------------------------------------+------------+" << std::endl;
    }

    void clearCart() {
        cart.clear();
    }
};

class Store {
private:
    std::vector<Item> inventory;
    std::vector<User> users;
    const std::string filename = "store_data.txt";

    void saveData() {
        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Unable to open file for writing: " + filename);
        }
        for (const auto& item : inventory) {
            file << "I," << item.name << "," << item.price << "," << item.quantity << "\n";
        }
        for (const auto& user : users) {
            file << "U," << user.username << "," << user.password << "," << user.isOwner << "\n";
        }
    }

    void loadData() {
        std::ifstream file(filename);
        if (!file) {
            std::cout << "No existing data file found. Starting with an empty store." << std::endl;
            return;
        }
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type, name, password;
            double price;
            int quantity;
            bool isOwner;

            std::getline(iss, type, ',');
            if (type == "I") {
                std::getline(iss, name, ',');
                iss >> price;
                iss.ignore();
                iss >> quantity;
                inventory.emplace_back(name, price, quantity);
            } else if (type == "U") {
                std::getline(iss, name, ',');
                std::getline(iss, password, ',');
                iss >> isOwner;
                users.emplace_back(name, password, isOwner);
            }
        }
    }

public:
    Store() {
        try {
            loadData();
        } catch (const std::exception& e) {
            std::cerr << "Error loading data: " << e.what() << std::endl;
        }
    }

    ~Store() {
        try {
            saveData();
        } catch (const std::exception& e) {
            std::cerr << "Error saving data: " << e.what() << std::endl;
        }
    }

    void addUser(const std::string& username, const std::string& password, bool isOwner) {
        users.emplace_back(username, password, isOwner);
    }

    User* login(const std::string& username, const std::string& password) {
        for (auto& user : users) {
            if (user.username == username && user.password == password) {
                return &user;
            }
        }
        return nullptr;
    }

    void addItem(const std::string& name, double price, int quantity) {
        inventory.emplace_back(name, price, quantity);
    }

    void displayInventory() const {
        std::cout << "\n+-------------------+------------+----------+" << std::endl;
        std::cout << "| Item Name         | Price      | Quantity |" << std::endl;
        std::cout << "+-------------------+------------+----------+" << std::endl;
        for (const auto& item : inventory) {
            std::cout << "| " << std::left << std::setw(17) << item.name.substr(0, 17) << " | "
                      << std::right << std::setw(10) << std::fixed << std::setprecision(2) << item.price << " | "
                      << std::setw(8) << item.quantity << " |" << std::endl;
        }
        std::cout << "+-------------------+------------+----------+" << std::endl;
    }

    void updateInventory(const std::string& name, int quantity) {
        for (auto& item : inventory) {
            if (item.name == name) {
                item.quantity += quantity;
                std::cout << "Inventory updated. New quantity for " << name << ": " << item.quantity << std::endl;
                return;
            }
        }
        std::cout << "Item not found in inventory." << std::endl;
    }

    bool buyItem(User& user, const std::string& name, int quantity) {
        for (auto& item : inventory) {
            if (item.name == name) {
                if (item.quantity >= quantity) {
                    item.quantity -= quantity;
                    user.addToCart(item, quantity);
                    std::cout << "Added " << quantity << " " << name << "(s) to your cart." << std::endl;
                    return true;
                } else {
                    std::cout << "Sorry, not enough stock. Available: " << item.quantity << std::endl;
                    return false;
                }
            }
        }
        std::cout << "Item not found in inventory." << std::endl;
        return false;
    }

    void checkout(User& user) {
        if (user.cart.empty()) {
            std::cout << "Your cart is empty. Nothing to checkout." << std::endl;
            return;
        }

        std::cout << "\nChecking out..." << std::endl;
        user.viewCart();
        std::cout << "\nThank you for your purchase!" << std::endl;
        user.clearCart();
    }
};

void displayMainMenu() {
    std::cout << "\n===================================" << std::endl;
    std::cout << "          * Welcome to *          " << std::endl;
    std::cout << "   ******** The Bazaar ********" << std::endl;
    std::cout << "      ~ Your Shopping Haven ~      " << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "1. Login" << std::endl;
    std::cout << "2. Sign Up" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "===================================" << std::endl;
}

void displayUserMenu(bool isOwner) {
    std::cout << "\n===================================" << std::endl;
    std::cout << "        *** USER MENU ***         " << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "1. View Inventory" << std::endl;
    if (isOwner) {
        std::cout << "2. Add Item" << std::endl;
        std::cout << "3. Update Inventory" << std::endl;
        std::cout << "4. Exit" << std::endl;
    } else {
        std::cout << "2. Buy Item" << std::endl;
        std::cout << "3. View Cart" << std::endl;
        std::cout << "4. Checkout" << std::endl;
        std::cout << "5. Exit" << std::endl;
    }
    std::cout << "===================================" << std::endl;
}

int main() {
    Store store;

    while (true) {
        displayMainMenu();
        std::cout << "Enter your choice: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            std::string username, password;
            std::cout << "Enter username: ";
            std::getline(std::cin, username);
            std::cout << "Enter password: ";
            std::getline(std::cin, password);

            User* user = store.login(username, password);
            if (user) {
                std::cout << "Welcome, " << user->username << "!" << std::endl;
                while (true) {
                    displayUserMenu(user->isOwner);
                    std::cout << "Enter your choice: ";
                    int userChoice;
                    std::cin >> userChoice;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    if (userChoice == 1) {
                        store.displayInventory();
                    } else if (userChoice == 2 && user->isOwner) {
                        std::string name;
                        double price;
                        int quantity;
                        std::cout << "Enter item name: ";
                        std::getline(std::cin, name);
                        std::cout << "Enter item price: ";
                        std::cin >> price;
                        std::cout << "Enter item quantity: ";
                        std::cin >> quantity;
                        store.addItem(name, price, quantity);
                        std::cout << "Item added successfully!" << std::endl;
                    } else if (userChoice == 3 && user->isOwner) {
                        std::string name;
                        int quantity;
                        std::cout << "Enter item name: ";
                        std::getline(std::cin, name);
                        std::cout << "Enter quantity change (positive to add, negative to remove): ";
                        std::cin >> quantity;
                        store.updateInventory(name, quantity);
                    } else if (userChoice == 2 && !user->isOwner) {
                        std::string name;
                        int quantity;
                        std::cout << "Enter item name to buy: ";
                        std::getline(std::cin, name);
                        std::cout << "Enter quantity: ";
                        std::cin >> quantity;
                        store.buyItem(*user, name, quantity);
                    } else if (userChoice == 3 && !user->isOwner) {
                        user->viewCart();
                    } else if (userChoice == 4 && !user->isOwner) {
                        store.checkout(*user);
                    } else if ((userChoice == 4 && user->isOwner) || (userChoice == 5 && !user->isOwner)) {
                        break;
                    } else {
                        std::cout << "Invalid choice. Please try again." << std::endl;
                    }
                }
            } else {
                std::cout << "Invalid username or password." << std::endl;
            }
        } else if (choice == 2) {
            std::string username, password;
            char userType;
            std::cout << "Enter username: ";
            std::getline(std::cin, username);
            std::cout << "Enter password: ";
            std::getline(std::cin, password);
            std::cout << "Enter user type (O for Owner, C for Customer): ";
            std::cin >> userType;
            store.addUser(username, password, userType == 'O' || userType == 'o');
            std::cout << "User registered successfully!" << std::endl;
        } else if (choice == 3) {
            std::cout << "Thank you for visiting The Bazaar! See you next time!" << std::endl;
            break;
        } else {
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }

    return 0;
}