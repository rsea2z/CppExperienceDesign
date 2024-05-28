/*
通讯录系统
1. 实现增删改查, 保存到文件, 从文件加载
2. C++面向对象编程 析构函数, 继承, 虚函数
3. STL高效使用 vector, string, fstream, sstream
4. 特殊情况处理 无/重复联系人, 无文件, 无效输入
5. 异常处理 throw, try, catch
6. 输出清晰 有提示选项
7. 文件保存格式csv, 文件冗余少, 便于人工查看, 便于程序解析

Author: 刘俊杰
Date: 2024-05-28
*/
#include <iostream> // 输入输出流
#include <fstream> // 文件流
#include <vector> // STL容器
#include <string> // STL字符串
#include <sstream> // 字符串流(用于分割字符串)
#include <algorithm> // 算法(用于查找copy_if)
#include <iomanip> // 控制格式(用于setw)
#include <functional> // 函数对象(用于谓词查询)

// 人员类
class Contact {
public:
    std::string name; // 姓名
    std::string gender; // 性别
    std::string phone; // 电话
    std::string class_name; // 班级
    std::string note; // 备注

    // 构造函数
    Contact() = default;
    Contact(const std::string& n, const std::string& g, const std::string& p, const std::string& c, const std::string& no)
        : name(n), gender(g), phone(p), class_name(c), note(no) {}

    // 重载输出运算符
    friend std::ostream& operator<<(std::ostream& os, const Contact& c) {
        // 输出联系人信息(设置宽度)
        os << std::left << std::setw(20) << c.name 
           << std::setw(10) << c.gender 
           << std::setw(20) << c.phone 
           << std::setw(20) << c.class_name 
           << std::setw(20) << c.note;
        return os;
    }
};

// 通讯录基类
class AddressBookBase {
protected:
    std::vector<Contact> contacts;

    // 获取谓词
    using Predicate = std::function<bool(const Contact&, const std::string&)>;
    Predicate getPredicate(const std::string& type) const {
        if (type == "name" or type == "Name" or type == "N" or type == "n") {
            return [](const Contact& c, const std::string& value) { return c.name == value; };
        } else if (type == "gender" or type =="Gender" or type == "G" or type == "g") {
            return [](const Contact& c, const std::string& value) { return c.gender == value; };
        } else if (type == "phone" or type == "Phone" or type == "P" or type == "p") {
            return [](const Contact& c, const std::string& value) { return c.phone == value; };
        } else if (type == "class" or type == "Class" or type == "C" or type == "c") {
            return [](const Contact& c, const std::string& value) { return c.class_name == value; };
        } else {
            throw std::invalid_argument("[ERROR] 没有这个类型的谓词");
        }
    }

// 基类创建纯虚函数, 用于派生类实现
public:
    // 添加联系人
    virtual void addContact() = 0;

    // 删除联系人
    virtual void deleteContact(const std::string& type, const std::string& value) = 0;

    // 修改联系人
    virtual void modifyContact(const std::string& type, const std::string& value) = 0;

    // 查找联系人
    virtual std::vector<Contact> findContacts(const std::string& type, const std::string& value) const = 0;
};

// 通讯录类
class AddressBook : public AddressBookBase {
private:
    // 抽象新建联系人时的录入所有信息
    Contact inputContact() {
        std::string name;
        std::string gender;
        std::string phone;
        std::string class_name;
        std::string note;
        std::cout << "[+] 输入姓名: ";
        std::cin >> name;
        std::cout << "[+] 输入性别: ";
        std::cin >> gender;
        std::cout << "[+] 输入电话: ";
        std::cin >> phone;
        std::cout << "[+] 输入班级: ";
        std::cin >> class_name;
        std::cout << "[+] 输入备注: ";
        std::cin >> note;
        return Contact(name, gender, phone, class_name, note);
    }

    // 处理修改联系人时, 选择仅修改某一值
    void modifyContactValue(Contact& contact) {
        std::cout << "[+] 输入要修改的值：\n";
        std::cout << "[1] 姓名\n"
                  << "[2] 性别\n"
                  << "[3] 电话\n"
                  << "[4] 班级\n"
                  << "[5] 备注\n"
                  << "[6] 所有\n"
                  << "[7] 退出\n"
                  << "[+] 请输入你的选择：";
        int choice;
        std::cin >> choice;
        std::string value;
        switch (choice) {
            case 1:
                std::cout << "[+] 输入新的姓名：";
                std::cin >> value;
                contact.name = value;
                break;
            case 2:
                std::cout << "[+] 输入新的性别：";
                std::cin >> value;
                contact.gender = value;
                break;
            case 3:
                std::cout << "[+] 输入新的电话：";
                std::cin >> value;
                contact.phone = value;
                break;
            case 4:
                std::cout << "[+] 输入新的班级：";
                std::cin >> value;
                contact.class_name = value;
                break;
            case 5:
                std::cout << "[+] 输入新的备注：";
                std::cin >> value;
                contact.note = value;
                break;
            case 6:
                contact = inputContact();
                break;
            case 7:
                break;
            default:
                std::cout << "[ERROR] 无效的选择\n";
        }
    }
        
public:
    // 添加联系人
    void addContact() override {
        contacts.push_back(inputContact());
    }

    // 删除联系人
    void deleteContact(const std::string& type, const std::string& value) override {
        // 获取谓词
        auto predicate = getPredicate(type);
        // 查找匹配的联系人
        std::vector<std::vector<Contact>::iterator> to_delete;
        for (auto it = contacts.begin(); it != contacts.end(); ++it) {
            if (predicate(*it, value)) {
                to_delete.push_back(it);
            }
        }
        // 处理匹配的联系人
        if (to_delete.size() > 1) {
            std::cout << "[!] 找到多个联系人，请选择要删除的联系人的序号：\n";
            for (size_t i = 0; i < to_delete.size(); ++i) {
                std::cout << "[" << i << "] " << *to_delete[i] << "\n";
            }
            size_t index;
            std::cin >> index;
            if (index < to_delete.size()) {
                contacts.erase(to_delete[index]);
            } else {
                std::cout << "[ERROR] 无效的序号\n";
            }
        } else if (to_delete.size() == 1) {
            contacts.erase(to_delete[0]);
        } else {
            std::cout << "[ERROR] 未找到联系人：" << value << "\n";
        }
    }

    // 修改联系人
    void modifyContact(const std::string& type, const std::string& value) override {
        // 获取谓词
        auto predicate = getPredicate(type);
        // 查找匹配的联系人
        std::vector<std::vector<Contact>::iterator> to_modify;
        for (auto it = contacts.begin(); it != contacts.end(); ++it) {
            if (predicate(*it, value)) {
                to_modify.push_back(it);
            }
        }
        // 处理匹配的联系人
        if (to_modify.size() > 1) {
            std::cout << "[!] 找到多个联系人，请选择要删除的联系人的序号：\n";
            for (size_t i = 0; i < to_modify.size(); ++i) {
                std::cout << "[" << i << "] " << *to_modify[i] << "\n";
            }
            size_t index;
            std::cin >> index;
            if (index < to_modify.size()) {
                modifyContactValue(*to_modify[index]);
            } else {
                std::cout << "[ERROR] 无效的序号\n";
            }
        } else if (to_modify.size() == 1) {
            modifyContactValue(*to_modify[0]);
        } else {
            std::cout << "[ERROR] 未找到联系人：" << value << "\n";
        }
    }

    // 查找联系人
    std::vector<Contact> findContacts(const std::string& type, const std::string& value) const override {
        // 查找联系人
        std::vector<Contact> found_contacts;
        // 获取谓词
        auto predicate = getPredicate(type);
        // 使用copy_if复制符合条件的联系人
        std::copy_if(contacts.begin(), contacts.end(), std::back_inserter(found_contacts), 
                     [&](const Contact& c) { return predicate(c, value); });
        return found_contacts;
    }

    // 保存到文件
    void saveToFile(const std::string& filename) const {
        // 如果通讯录为空，提示无联系人
        if (contacts.empty()) {
            std::cerr << "[ERROR] 通讯录为空，无法保存\n";
            return;
        }
        // 如果文件名不是csv格式，添加csv后缀
        std::string mutableFilename = filename; // 为了修改文件名，拷贝一份mutableFilename
        if (mutableFilename.substr(mutableFilename.find_last_of(".") + 1) != "csv") {
            mutableFilename += ".csv";
        }
        // 打开文件流
        std::ofstream file(mutableFilename);
        if (!file) {
            std::cerr << "[ERROR] 无法打开文件：" << mutableFilename << "\n";
            return;
        }
        // 写入表头
        file << "姓名,性别,电话,班级,备注\n";
        // 写入联系人信息
        for (const auto& contact : contacts) {
            file << contact.name << "," << contact.gender << "," << contact.phone << "," << contact.class_name << "," << contact.note << "\n";
        }
        file.close();
    }

    // 从文件加载
    void loadFromFile(const std::string& filename) {
        // 打开文件流
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "[ERROR] 无法打开文件：" << filename << "\n";
            return;
        }
        // 清空通讯录
        contacts.clear();
        // 读取文件内容
        std::string line;
        // 跳过表头
        std::getline(file, line);
        // 逐行读取联系人信息
        while (std::getline(file, line)) {
            // 使用istringstream分割每行内容
            std::istringstream iss(line);
            std::string name, gender, phone, class_name, note;
            if (std::getline(iss, name, ',') && 
                std::getline(iss, gender, ',') && 
                std::getline(iss, phone, ',') && 
                std::getline(iss, class_name, ',') && 
                std::getline(iss, note, ',')) {
                contacts.emplace_back(name, gender, phone, class_name, note);
            } else { // 如果解析失败，输出错误信息
                std::cerr << "[ERROR] 无法解析行：" << line << "\n";
            }
        }
        file.close();
    }
    
    // 显示所有联系人
    void display() const {
        // 如果通讯录为空，提示无联系人
        if (contacts.empty()) {
            std::cerr << "[ERROR] 通讯录为空\n";
            return;
        }
        // 输出表头
        std::cout << "\n[INFO] 所有联系人：\n";
        std::cout << std::left << std::setw(22) << "姓名" 
                  << std::setw(12) << "性别" 
                  << std::setw(22) << "电话" 
                  << std::setw(22) << "班级" 
                  << std::setw(22) << "备注" << "\n";
        // 输出联系人信息
        for (const auto& contact : contacts) {
            std::cout << contact << "\n";
        }
    }

    // 析构函数
    virtual ~AddressBook() {
        contacts.clear();
        std::cout << "[INFO] 通讯录已摧毁\n";
    }
};

// 显示菜单
void showMenu() {
    std::cout << "\n[1] 添加联系人\n"
              << "[2] 删除联系人\n"
              << "[3] 修改联系人\n"
              << "[4] 查找联系人\n"
              << "[5] 显示所有联系人\n"
              << "[6] 保存到文件\n"
              << "[7] 从文件加载\n"
              << "[8] 退出\n"
              << "[+] 请输入你的选择：";
}

int main() {
    system("cls");
    std::cout << "-------------欢迎使用通讯录系统系统------------\n";
    std::cout << " _    _ _____ _     _____ ________  ___ _____ \n";
    std::cout << "| |  | |  ___| |   /  __ \\  _  |  \\/  ||  ___|\n";
    std::cout << "| |  | | |__ | |   | /  \\/ | | | .  . || |__  \n";
    std::cout << "| |/\\| |  __|| |   | |   | | | | |\\/| ||  __| \n";
    std::cout << "\\  /\\  / |___| |___| \\__/\\ \\_/ / |  | || |___ \n";
    std::cout << " \\/  \\/\\____/\\_____/\\____/\\___/\\_|  |_/\\____/ \n";
    std::cout << "-------------作者: 刘俊杰 计科2201-------------\n";
    AddressBook addressBook;
    int choice;
    std::string name, gender, phone, class_name, note, filename, type, value;

    // do-while循环显示菜单, 选择大于8直接退出
    do {
        showMenu();
        std::cin >> choice;

        switch (choice) {
            case 1:
                addressBook.addContact();
                std::cout << "[INFO] 联系人已添加\n";
                break;
            case 2:
                std::cout << "[+] 输入查找要删除人员的类型(Name/Gander/Phone/Class): ";
                std::cin >> type;
                std::cout << "[+] 输入查找要删除人员该类型的值: ";
                std::cin >> value;
                try {
                    addressBook.deleteContact(type, value);
                } catch (const std::invalid_argument& e) {
                    std::cerr << e.what() << "\n";
                    break;
                }
                std::cout << "[INFO] 联系人已删除\n";
                break;
            case 3:
                std::cout << "[+] 输入查找要修改人员的类型(Name/Gander/Phone/Class): ";
                std::cin >> type;
                std::cout << "[+] 输入查找要修改人员该类型的值: ";
                std::cin >> value;
                try {
                    addressBook.modifyContact(type, value);
                } catch (const std::invalid_argument& e) {
                    std::cerr << e.what() << "\n";
                    break;
                }
                std::cout << "[INFO] 联系人已修改\n";
                break;
            case 4:
                {
                std::cout << "[+] 请输入要进行查找的类型(Name/Gander/Phone/Class)";
                std::cin >> type;
                std::cout << "[+] 请输入要查找人员该类型的值：";
                std::cin >> value;
                try {
                    std::vector<Contact> found_contacts = addressBook.findContacts(type, value);
                } catch (const std::invalid_argument& e) {
                    std::cerr << e.what() << "\n";
                    break;
                }std::vector<Contact> found_contacts = addressBook.findContacts(type, value);
                if (found_contacts.empty()) {
                    std::cout << "[ERROR] 未找到联系人：" << value << "\n";
                } else {
                    std::cout << "[INFO] 找到的联系人：\n";
                    for (const auto& contact : found_contacts) {
                        std::cout << contact << "\n";
                    }
                }
                break;
                }
            case 5:
                addressBook.display();
                break;
            case 6:
                std::cout << "[+] 请输入文件名：";
                std::cin >> filename;
                addressBook.saveToFile(filename);
                break;
            case 7:
                std::cout << "[+] 请输入文件名：";
                std::cin >> filename;
                addressBook.loadFromFile(filename);
                break;
            case 8:
                std::cout << "[-] 通讯录系统已退出\n";
                break;
            default:
                std::cout << "[ERROR] 无效的选择\n";
        }
    } while (choice != 8);

    return 0;
}