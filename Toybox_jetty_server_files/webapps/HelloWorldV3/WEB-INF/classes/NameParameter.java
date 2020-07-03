import java.io.FileWriter; 
import java.io.File;  // Import the File class
import java.io.PrintWriter;
import java.io.IOException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class NameParameter extends HttpServlet {

	@Override
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException {

		String name = request.getParameter("name");
		
		PrintWriter out = response.getWriter();
		writeNameToFile(name);
		out.println("<h1>Hello " + name + "</h1>");
		out.println("<p>Nice to meet you!</p>");
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
}