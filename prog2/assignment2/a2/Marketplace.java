package assignment2.a2;

import assignment2.a2.auth.User;
import assignment2.a2.offerings.Item;

public class Marketplace {
    private User[] users = new User[10];

    public boolean addUser(User user) {
        for (int i = 0; i < users.length; i++) {
            if (users[i] == null) {
                users[i] = user;
                return true;
            }
        }
        return false;
    }

    public boolean removeUser(User user) {
        for (int i = 0; i < users.length; i++) {
            if (users[i] == null) {
                users[i] = user;
                return true;
            }
        }
        return false;
    }

    public String str() {
        String s = "";
        for (User user : users) {
            if (user != null) {
                s += "[User: " + user.getUsername() + "]\n";
                for (Item item : user.getItems()) {
                    if (item != null)
                        s += "\t<Item : " + item.getName() + ", Price: " + item.getPrice() + ", Seller: " + item.getSeller() + ", Description: " + item.getDescription() + ">\n";
                }
            }
        }


        return s;
    }
}
