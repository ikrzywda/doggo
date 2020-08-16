package filerunnerone;


import java.io.*;
import java.util.*;


public class Body {
	
	public static final String COMMA_DELIMITER = ",";
	public static final String CSV_FILE = "src/test/resources/user.csv";		// TEST NAME, REQUIRES REAL LOCATION OF CSV FILES
	public List<String> intlist;
	public ArrayList<Integer> truerealints = toInt();
	


	public Body() {
		
	}
	
	public void read() {
		
		List<List<String>> records = new ArrayList<>();
		try (Scanner scanner = new Scanner(new File("user.csv"));) {              
		    while (scanner.hasNextLine()) {
		        records.add(write(scanner.nextLine()));
		    }
		}
	
	
	}
	
	public List<String> write(String line) {
        List<String> values = new ArrayList<String>();
        try (Scanner rowScanner = new Scanner(line)) {
            rowScanner.useDelimiter(COMMA_DELIMITER);
            while (rowScanner.hasNext()) {
                values.add(rowScanner.next());
            }
        }
        this.intlist = values;
        return values;
	
	}
	

	
	public ArrayList<Integer> toInt() {
		
	ArrayList<Integer> realints = new ArrayList<Integer>();
		
		for(int i = 0 ; i < intlist.size() ; i++) {
			realints.add(Integer.parseInt(intlist.get(i)));
		}
		
		return realints;
	}
	
	public ArrayList<Integer> leaderboard() {
		
		Collections.sort(truerealints);
		
		return truerealints;
		
		
	}
}
