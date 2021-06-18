public class BinaryNode<Integer> {

    private int data;
    private BinaryNode<Integer> leftChild;
    private BinaryNode<Integer> rightChild;

    public BinaryNode(int d) {
        this.data = d;
        leftChild = null;
        rightChild = null;
    }
    public int getData() {
        return data;
    }
    public void setData(int d) {
        data = d;
    }
    public BinaryNode getLeft(){
        return leftChild;
    }
    public BinaryNode getRight() {
        return rightChild;
    }
    public void setLeftChild(BinaryNode b) {
        this.leftChild = b;
    }
    public void setRightChild(BinaryNode b) {
        this.rightChild = b;
    }
}
