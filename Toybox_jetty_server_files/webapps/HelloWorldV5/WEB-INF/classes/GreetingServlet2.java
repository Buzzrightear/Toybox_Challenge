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
	double toyBoxFillRatio = 0.75;
	boolean buttonOverride = false;
	ArrayList<String[]> mainTagList = new ArrayList<String[]>();
	//Note that I have put a space in front of each tag ID - need to change format of this data when posted from toybox scanner
	String [][] toyBoxArray = {
		{" 418610782", "Bear rattle"},
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
		
		if(buttonOverride){
			buttonOverride = false;
			processSweetBoxPOST(response);
		}
		//test if POST comes from sweetBox device:
		else if(postData.equals("UNLOCK_REQUEST")){
			processSweetBoxPOST(response);
		}
		//Else if it comes from button on website: 
		else if(postData.equals("BUTTON_REQUEST")){
			buttonOverride = true;
			processButtonRequest(response);
		}
		else{
			processToyBoxPOST(response, postData);
		}
		
	
	}

	// if the POST is from toybox device:
	public void processToyBoxPOST(HttpServletResponse response1,String postData1){
		//response goes back to client:
		try{
			response1.getOutputStream().println("Message No. " + counter + " - Testing this is what was sent: " + postData1);
			
			//received data gets added to the ArrayList declared above
			String [] tempTagArray = postData1.split(",",0);
			boolean contains = false;
			for (int m = 0; m<mainTagList.size();m++){
				if (mainTagList.get(m)[1].equals(tempTagArray[1])){
					mainTagList.remove(m);
					writeNameToFile(postData1);
					contains = true;
				}
			}
			if (contains == false){
				mainTagList.add(tempTagArray);
				writeNameToFile(postData1); //unknown if this works
			}
			counter = counter +1;
		}
		catch (final Exception e) {
			System.out.println("Could not send message to client");
		}
	}
	
	// if the POST is from sweetBox device:
	public void processSweetBoxPOST(HttpServletResponse response1){
		String tempMsg = "";
		double tempDouble = mainTagList.size()/toyBoxArray.length;
		boolean tempBool = (mainTagList.size()/toyBoxArray.length)>=toyBoxFillRatio;
		if((mainTagList.size()/toyBoxArray.length)>=toyBoxFillRatio) {//toybox has equal or greater toyBoxFillRatio of  registered toys in it 
			try{
				//response goes back to client to UNLOCK:
				tempMsg = "mainTagList size: " + mainTagList.size() + " toyBoxArray.length: " + toyBoxArray.length + " divided by each other = " + tempDouble + " toyBoxFillRatio: " + toyBoxFillRatio + " evals to: " + tempBool;
				response1.getOutputStream().println("UNLOCK");		
			}
			catch (final Exception e) {
				System.out.println("Could not send message to client");
			}
		}
		else{
			try{
				response1.getOutputStream().println("LOCK"); //*****************CHANGE TO 'UNLOCK' FOR TESTING
			}
			catch (final Exception e) {
				System.out.println("Could not send message to client");
			}
		}
	}
	
	//if POST is from website:
	public void processButtonRequest(HttpServletResponse response2){
		try{
				//response goes back to client to UNLOCK:
				response2.getOutputStream().println("Confirm button request received by server");		
		}
		catch (final Exception e) {
				System.out.println("Could not send message to client");
		}
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
