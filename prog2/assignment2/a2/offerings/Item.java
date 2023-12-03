package assignment2.a2.offerings;

import assignment2.a2.auth.User;

public class Item {
    private String name;
    private float price;
    private User seller;
    private String description;

    public Item(String n, float p, User s, String d) {
        this.name = n;
        this.price = p;
        this.seller = s;
        this.description = d;
    }

    public float getPrice() {
        return price;
    }
    public void setPrice(float price) {
        this.price = price;
    }

    public User getSeller() {
        return seller;
    }

    public void setSeller(User seller) {
        this.seller = seller;
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    public String toString() {
        StringBuilder s = new StringBuilder();
        s.append("\t<Item : ").append(name)
         .append(", Price: ").append(price)
         .append(", Seller: ").append(seller.getUsername())     // careful not to cause infinite recursion here by calling the user.
         .append(", Description: ").append(description)
         .append(">\n");

        return s.toString();
    }
}
