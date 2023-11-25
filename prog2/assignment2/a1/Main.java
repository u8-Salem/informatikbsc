package assignment2.a1;

import assignment2.a1.model.Tree;

public class Main {
    public static void main(String[] args) {
        Tree tree = new Tree(20);
        System.out.println(tree.str());

        tree.insertValue(30);
        System.out.println(tree.str());

        tree.insertValue(40);
        tree.insertValue(10);
        tree.insertValue(35);
        tree.insertValue(10);
        System.out.println(tree.str());

        System.out.println(tree.contains(40));
        System.out.println(tree.contains(21));

        Tree tree2 = new Tree(35);
        tree2.insertValue(10);
        tree2.insertValue(40);
        tree2.insertValue(20);
        tree2.insertValue(30);

        Tree tree3 = new Tree(35);
        tree3.insertValue(10);
        tree3.insertValue(40);
        tree3.insertValue(20);
        tree3.insertValue(31);

        System.out.println(tree.hasSameValues(tree2));
        System.out.println(tree.hasSameValues(tree3));

    }

}
