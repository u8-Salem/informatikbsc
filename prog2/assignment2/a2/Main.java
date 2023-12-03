package assignment2.a2;

import assignment2.a2.auth.User;
import assignment2.a2.offerings.Item;

public class Main {
    public static void main(String[] args) {
        Marketplace marketplace = new Marketplace();

        User user1 = new User("Sam", "1234");
        User user2 = new User("notSam", "4321");

        Item car = new Item("car", 2000.0F, null, "a blue car");
        Item cup = new Item("cup", 5.0F, null, "a nice cup");
        Item coffee = new Item("coffee", 2.99F, null, "nice coffee");
        Item chair = new Item("chair", 24.99F, null, "ugly chair");

        user1.addItem(car);
        user1.addItem(cup);
        user2.addItem(coffee);
        user2.addItem(chair);

        System.out.println(user1);
        user1.removeItem(car);
        System.out.println(user1);

        marketplace.addUser(user1);
        marketplace.addUser(user2);
        System.out.println(marketplace.str());
        marketplace.removeUser(user2);
        System.out.println(marketplace.str());


    }
}