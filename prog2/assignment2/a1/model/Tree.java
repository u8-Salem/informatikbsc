package assignment2.a1.model;

import jogamp.common.os.elf.SectionArmAttributes;

public class Tree {
    private int value;
    private Tree left;
    private Tree right;

    public Tree(int value) { this.value = value; }

    public String str() {
        return "<" +
                (this.left != null ? this.left.str() : " ") +
                "> <" +
                this.value +
                "> <" +
                (this.right != null ? this.right.str() : " ") +
                ">";
    }

    public void insertValue(int value) {
        if (this.value == value) { return; }

        if (this.value > value) {
            if (this.left != null) {
                this.left.insertValue(value);
            } else {
                this.left = new Tree(value);
            }
        } else {
            if (this.right != null) {
                this.right.insertValue(value);
            } else {
                this.right = new Tree(value);
            }
        }
    }

    public boolean contains(int value) {
        if (this.value == value) { return true; }

        if (this.value > value && this.left != null) {
            return this.left.contains(value);
        } else if (this.right != null) {
            return this.right.contains(value);
        }
        return false;
    }

    public boolean hasSameValues(Tree other) {
        if (!other.contains(this.value)) { return false; }

        if (this.left != null && !this.left.hasSameValues(other)) { return false; }
        if (this.right != null && !this.right.hasSameValues(other)) { return false; }

        return true;
    }
    public int getValue() { return this.value; }
    public Tree getLeft() { return this.left; }
    public Tree getRight() { return this.right; }
    public void setValue(int value) { this.value = value; }
    public void setLeft(Tree left) { this.left = left; }
    public void setRight(Tree right) { this.right = right;}

}
