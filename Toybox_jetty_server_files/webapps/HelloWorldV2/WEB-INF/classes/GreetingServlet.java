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

public class GreetingServlet extends HttpServlet {
	int counter = 0;
	ArrayList<String> mainTagList = new ArrayList<String>();
	
	@Override
	public void doPost(HttpServletRequest request, HttpServletResponse response) throws IOException, ServletException {
		
		//Attempt to get the post data:
		String postData = "";
		StringBuilder builder = new StringBuilder();
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
		
		
		//response goes back to client
		response.getOutputStream().println("Message No. " + counter + " - Testing this is what was sent: " + postData);
		
		mainTagList.add("postData is " + postData);

		writeNameToFile(postData);
		counter = counter +1;		
	}


	
	
	
		//Write value supplied in url to new file:
	public void writeNameToFile(String aString){
		try {
			FileWriter myWriter = new FileWriter("nameFile.txt");
			myWriter.write("Files in Java might be tricky, but it is fun enough!\n");
			myWriter.write("The name is " + aString + "\n");
			myWriter.write("The name " + aString + " is stupid \n");
			myWriter.close();
			System.out.println("Successfully wrote to the file.");
		} 
		catch (IOException e) {
			System.out.println("An error occurred.");
			e.printStackTrace();
		}
	}
	
	//This does show when you go to the /hello url (but not the list stuff)
	@Override
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException, ServletException {
		response.getOutputStream().println("<h1>Attempt at toybox Web App</h1>");
		response.getOutputStream().println("<hr/>");
		response.getOutputStream().println("Maintaglist size is " + mainTagList.size());
		mainTagList.add("Test entry");
		for(int i = 0; i < mainTagList.size(); i++){
			response.getOutputStream().println("<p>" + mainTagList.get(i) + "</p>");
		}
		response.getOutputStream().println("<hr/>");
		response.getOutputStream().println("End of page");
	}
}