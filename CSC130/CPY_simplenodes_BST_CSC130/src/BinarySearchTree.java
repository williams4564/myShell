
public class BinarySearchTree {
    private BinaryNode<Integer> root;
    private int height;

    public BinarySearchTree(BinaryNode r) {
        root = r;
        height = 0;
    }
    public int getRoot() {
        return root;
    }
    public int getHeight() {
        return height;
    }
    public BinaryNode<Integer> findMin(BinaryNode<Integer> n) {
        if(t = null) {
            return null;
        }
        else if (t.getLeft() == null) {
            return t;
        }
        else {
            return findMin(t.getLeft());
        }
    }
    public BinaryNode findMax(BinaryNode n) {
        if(n != null) {
            while (n.right != null){
                t= t.getRight();
            }
        }
        return t;
    }
    //public static Integer last_printed = null;
   // public static boolean checkBST()
    public boolean contains(Integer x, BinaryNode t) {
        if (n == null) return false;

        int compareResult = x.compareTo(t.getData());
        if(compareResult < 0) {
            return contains(x, t.getLeft());
        }
        else if(compareResult > 0) {
            return contains(x, t.getRight());
        } else {
            return true;
        }
    }
    public BinaryNode insert(Integer x, BinaryNode t) {
        if (t == null){
            return new BinaryNode(x, null, null);
        }
        int compareResult = x.compareTo(t.getData());

        if(compareResult < 0) {
            t.getLeft = insert(x, t.getLeft());
        }
        else if (compareResult > 0) {
            t.getRight() = insert(x, t.getRight());
        }
        else {
            //nothing
        }
        return t;
    }
    public BinaryNode remove(Integer x, BinaryNode t){
        if (t == null) return t;
        int compareResult = x.compareTo(t.getData());
    }

}