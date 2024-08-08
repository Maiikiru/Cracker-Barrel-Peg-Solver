package src;
class Main{
    //This quick helper class helps us create a LU Table quickly so we may hardcode it in board.h
    public static void main(String[] args){
        short [][][] arr = {
            {{1,3},{2,5}},
            {{3,6},{4,8}},
            {{4,7},{5,9}},
            {{4,5},{1,0},{6,10},{7,12}},
            {{7,11},{8,13}},
            {{4,3},{2,0},{8,12},{9,14}},
            {{7,8},{3,1}},
            {{8,9},{4,2}},
            {{7,6},{4,1}},
            {{8,7},{5,2}},
            {{11,12},{6,3}},
            {{12,13},{7,4}},
            {{11,10},{13,14},{7,3},{8,5}},
            {{12,11},{8,4}},
            {{13,12},{9,5}},
        };

        for(short i=0;i<arr.length;i++){
            System.out.print("{");
            for(short j=0;j<arr[i].length;j++){
                short expected = (short) (0x1 << arr[i][j][0] | 0x1 << i);
                short mask = (short) (expected | 0x1 << arr[i][j][1]);
                System.out.print("{");
                System.out.print(String.format("0x%04X", expected));
                System.out.print(",");
                System.out.print(String.format("0x%04X", mask));
                System.out.print("}");
                if (j != arr[i].length -1){
                    System.out.print(",");
                }
                
            }
            System.out.print("};\n");
        }
    }
}