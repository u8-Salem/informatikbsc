package assignment2.a2;

import assignment2.a2.auth.User;
import assignment2.a2.offerings.Item;

public class Marketplace {
    private User[] users = new User[10];

    public boolean removeUser(User user) {
        for (int i = 0; i < users.length; i++) {
            if (users[i] != null && users[i].equals(user)) {
                users[i] = null;
                return true;
            }
        }
        return false;
    }

    public boolean addUser(User user) {
        for (int i = 0; i < users.length; i++) {
            if (users[i] == null) {
                users[i] = user;
                return true;
            }
        }
        return false;
    }

    public String str() {
        StringBuilder s = new StringBuilder();
        for (User user : users) {
            if (user != null) {
                s.append(user);     // .toString is called implicitly
            }
        }

        return s.toString();
    }
}
