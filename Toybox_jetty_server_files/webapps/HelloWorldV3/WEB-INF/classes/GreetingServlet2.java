import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.FileWriter; 
import java.io.File;  // Import the File class
import java.io.PrintWriter;
import java.util.ArrayList;
import java.io.BufferedReader;
import java.util.*;
import java.io.*; 
 

public class GreetingServlet2 extends HttpServlet {
	int counter = 0;
	ArrayList<String[]> mainTagList = new ArrayList<String[]>();
	//Note that I have put a space in front of each tag ID - need to change format of this data when posted from toybox scanner
	String [][] toyBoxArray = {
		{" 418610782", "Be	ar rattle"},
		{" 419710882", "Fish pet thing"},
		{" 420310882", "Digger"},
		{" 420910882", "Doctor's kit"}
	};
	
	@Override
	public void doPost(HttpServletRequest request, HttpServletResponse response) throws IOException, ServletException {
		
		//Attempt to get the post data:
		String postData = "";
		StringBuilder builder = new StringBuilder();
		
		//request.getReader() gets the body of the post and puts it in an instance of BufferedReader. StringBuilder then makes it into a string
		try (BufferedReader reader = request.getReader()) {
			if (reader == null) {
				System.out.println("Request body could not be read because it's empty.");
			}
			String line;
			while ((line = reader.readLine()) != null) {
				builder.append(line);
			}
			postData = builder.toString();
		} 
		catch (final Exception e) {
			System.out.println("Could not obtain the saml request body from the http request");
		}
		
		
		//response goes back to client:
		response.getOutputStream().println("Message No. " + counter + " - Testing this is what was sent: " + postData);
		
		//received data gets added to the ArrayList declared above
		String [] tempTagArray = postData.split(",",0);
		boolean contains = false;
		for (int m = 0; m<mainTagList.size();m++){
			if (mainTagList.get(m)[1].equals(tempTagArray[1])){
				mainTagList.remove(m);
				writeNameToFile(postData);
				contains = true;
			}
		}
		if (contains == false){
			mainTagList.add(tempTagArray);
			writeNameToFile(postData); //unknown if this works
		}
		counter = counter +1;
	}


	
	
	
		//Write value supplied in url to new file:
	public void writeNameToFile(String aString){
		try {
			FileWriter myWriter = new FileWriter("nameFile.txt");
			myWriter.write(aString);
			myWriter.close();
			System.out.println("Successfully wrote to the file.");
		} 
		catch (IOException e) {
			System.out.println("An error occurred.");
			e.printStackTrace();
		}
	}
	
	//This does show when you go to the /hello url 
	@Override
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException, ServletException {
		//response.getOutputStream().println() will put stuff on the relevant webpage for this file (see the web.xml file for servlet mapping to urls)
		response.getOutputStream().println("<h2>Total toys registered to this toybox: " + mainTagList.size() + "</h2>");
		if (mainTagList.size() > 1){
			response.getOutputStream().println("<h2>Current toys in the toybox are: </h2>");
			//Iterates over list and puts each row of ArrayList on to web page
			for(int i = 0; i < mainTagList.size(); i++){
				String tempString0 = "null";
				for (int k =0; k<toyBoxArray.length;k++){
					if (toyBoxArray[k][0].equals(mainTagList.get(i)[1])){
						tempString0 = toyBoxArray[k][1];
					}			
				}
				response.getOutputStream().println("<p> Tag ID: " + mainTagList.get(i)[1] + " associated with the toy known as: \"" + tempString0  + "\"</p>");
			}
			response.getOutputStream().println("<h3>Toybox has " + mainTagList.size() + " out of " + toyBoxArray.length + " toys in it at the moment</h3>");
		}
		else if (mainTagList.size() == 1){
			response.getOutputStream().println("<h2>Current toy in the toybox is: </h2>");
			//Iterates over list and puts each row of ArrayList on to web page
			for(int j = 0; j < mainTagList.size(); j++){
				String tempString1 = "null";
				for (int k =0; k<toyBoxArray.length;k++){
					if (toyBoxArray[k][0].equals(mainTagList.get(j)[1])){
						tempString1 = toyBoxArray[k][1];
					}			
				}
				response.getOutputStream().println("<p> Tag ID: " + mainTagList.get(j)[1] + " associated with the toy known as: \"" + tempString1  +  "\"</p>");
			}
			response.getOutputStream().println("<h3>Toybox has " + mainTagList.size() + " out of " + toyBoxArray.length + " toys in it at the moment</h3>");
		}
		else{
			response.getOutputStream().println("<h2>Toybox is empty</h2>");
		} 
	}
}