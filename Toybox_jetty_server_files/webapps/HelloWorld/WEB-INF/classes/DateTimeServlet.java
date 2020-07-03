import java.io.PrintWriter;
import java.io.IOException;
import java.util.Date;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class DateTimeServlet extends HttpServlet {

	@Override
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException {

		Date now = new Date();

		PrintWriter out = response.getWriter();
		out.println("<p>Current time: " + now.toString() + "</p>");
	}
}