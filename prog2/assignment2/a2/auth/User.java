package assignment2.a2.auth;

import assignment2.a2.offerings.Item;

public class User {
    private String username;
    private String password;
    private Item[] items = new Item[10];

    public User(String username, String password) {
        this.username = username;
        this.password = password;
    }

    public boolean addItem(Item item) {
        for (int i = 0; i < items.length; i++) {
            if (items[i] == null) {
                items[i] = item;
                items[i].setSeller(this);
                return true;
            }
        }
        return false;
    }

    public boolean removeItem(Item item) {
        for (int i = 0; i < items.length; i++) {
            if (items[i] != null && items[i].equals(item)) {
                items[i] = null;
                return true;
            }
        }
        return false;
    }

    public String toString() {
        StringBuilder s = new StringBuilder();
        s.append("[User: ").append(username).append("]\n");
        for (Item item : items) {
            if (item != null)
                s.append(item);     // .toString is called implicitly
        }

        return s.toString();
    }

    public String getUsername() {
        return username;
    }
    public String getPassword() {
        return password;
    }
    public Item[] getItems() {
        return items;
    }
    public void setUsername(String username) {
        this.username = username;
    }
    public void setPassword(String password) {
        this.password = password;
    }
    public void setItems(Item[] items) {
        this.items = items;
    }
}