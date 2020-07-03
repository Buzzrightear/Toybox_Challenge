import java.io.IOException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpServletRequest;

@WebServlet("/time")
public class UnixTimeServlet extends HttpServlet {

  @Override
  public void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException {
    long unixTimeSeconds = System.currentTimeMillis() / 1000;

    response.setContentType("text/html;");
    response.getWriter().println(unixTimeSeconds);
  }
}