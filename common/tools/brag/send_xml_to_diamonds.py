command_help = """
Send XML data from file to Diamonds. v.2.0
Use:
    send_xml_to_diamonds.py options
    
    Mandatory options:
    -u    Url
    -f    path of XML file
    
    Optional options:
    -m    Send only mail, without POST connection. Recommend only,
          when direct POST connection is not available.
    -o    Mail server, not needed inside Nokia intranet.
    -h    Help, prints this help text.
    
    Examples:
    Sending only by mail, without POST. (not recommended)
        send_xml_to_diamonds.py -u http://diamonds.nmp.nokia.com/diamonds/builds/ -f c:\\build.xml -m buildtoolsautomation@nokia.com
    
    Sending a new build to release instance of Diamonds
        send_xml_to_diamonds.py -u http://diamonds.nmp.nokia.com/diamonds/builds/ -f c:\\build.xml
    
    Updating test results to existing build
        send_xml_to_diamonds.py -u http://diamonds.nmp.nokia.com/diamonds/builds/123/ -f c:\\test.xml
    
    Sending data for Relative Change in SW Asset metrics
        send_xml_to_diamonds.py -u http://diamonds.nmp.nokia.com/diamonds/metrics/ -f c:\\relative.xml
    
    Sending data for Function Coverage of Releases
        send_xml_to_diamonds.py -u http://diamonds.nmp.nokia.com/diamonds/tests/coverage/ -f c:\\coverage.xml
    
    Note: If you want to send XML to development version of Diamonds in testing purposes, use
    address: trdeli02.nmp.nokia.com:9001 in the server address:
        send_xml_to_diamonds.py -u http://trdeli02.nmp.nokia.com:9001/diamonds/builds/ -f c:\\build.xml
    
    For local Diamonds development:
        send_xml_to_diamonds.py -u http://127.0.0.1:8000/diamonds/builds/ -f c:\\build.xml
"""

from httplib import *
import os, sys, time, re


def send_email(subject, body, sender, receivers, encoding, mail_server):
    """
    Create an email message as MIMEText instance.
    """
    from email.Header import Header
    from email.MIMEText import MIMEText
    from email.Utils import parseaddr, formataddr
    import smtplib
    
    msg = MIMEText(body, "plain", encoding)
    msg["To"] = Header(u", ".join(receivers), encoding)
    msg["Subject"] = Header(subject, encoding)
    
    smtp = smtplib.SMTP() 
    smtp.connect(mail_server)
    smtp.sendmail(sender, receivers, msg.as_string())
    smtp.close()

def get_username():
    platform = sys.platform
    if platform == "win32":
        return os.getenv("USERNAME")
    else:
        return os.getlogin()

def get_mail_subject(sender, server, url):
    return "[DIAMONDS_DATA] %s>>>%s>>>%s" % (sender, server, url)

def get_response_message(response):
    return "Response status:%s \
    \nResponse reason:%s\n" \
           % (response.status, response.reason)

def get_process_time(total_time):
    if total_time<=60:
        return  "%s seconds" % round(total_time, 1)
    else:
        return "%s minutes and %s seconds" % (int(total_time/60), round((total_time%60), 1))

def safe_pop():
    try:
        return sys.argv.pop(0)
    except IndexError:
        sys.exit("Syntax Error!\n%s" % command_help)

def get_server_from_url(url):
    # Remove 'http://' if any
    try:
        return url.replace("http://", "").split("/")[0]
    except IndexError:
        sys.exit("Invalid URL: %s" % url)

def get_path_from_url(url):
    try:
        path = url.replace("http://", "").split("/")
        # Discard server address
        _ = path.pop(0)
        path = "/" + "/".join(path)
        # Append slash if needed
        if path[-1] != "/":
            path = path + "/"
        return path
    except IndexError:
        sys.exit("Invalid URL: %s" % url)

def main():
    if len(sys.argv) < 2:
        sys.exit(command_help)
    start_time = time.time()
    server_valid = False
    path_valid = False
    sfile_valid = False
    mail_address = None
    mail_server_address = "smtp.nokia.com"
    _ = sys.argv.pop(0)
    
    while sys.argv:
        parameter = sys.argv.pop(0)
        if re.search('^-', parameter):
            if parameter == '-u':
                url = safe_pop()
                server = get_server_from_url(url)
                server_valid = True
                path = get_path_from_url(url)
                path_valid = True
            elif parameter == '-f':
                source_file = safe_pop()
                sfile_valid = True
                try:
                    xml = open(source_file).read()
                except:
                    sys.exit("Can not open the file %s" % source_file)
            elif parameter == '-m':
                mail_address = sys.argv.pop(0)
            elif parameter == '-o':
                mail_server_address = sys.argv.pop(0)
            elif parameter == '-h':
                sys.exit("HELP:\n %s" % (command_help))
            else:
                sys.exit("Incorrect parameter! %s" % (parameter) + command_help )
        else:
            sys.exit("Incorrect parameter! %s" % (parameter) + command_help)
    if not server_valid or not path_valid or not sfile_valid:
        sys.exit("Too few parameters. \n%s" % command_help)
    
    diamonds_mail_box      = "diamonds@diamonds.nmp.nokia.com"
    import_failed_message  = "XML was not sent successfully to Diamonds via REST interface!\n"
    import_succeed_message = "XML was sent successfully to Diamonds via REST interface.\n"
    mail_sent_message      = "XML was sent to Diamonds by mail. Scheduled script will try to import it to Diamonds. If you can not see data soon in Diamonds, please contact to Diamonds developers.\n"
    
    if not mail_address:
        connection = HTTPConnection(server)
        
        try:
            connection.request("POST", path, xml)
        except:
            print "Can not connect to the server %s\n" % server
            sender = get_username()
            send_email(get_mail_subject(sender, server, path), xml, sender, [diamonds_mail_box], "latin-1", mail_server_address)
            sys.exit(mail_sent_message)
        
        response = connection.getresponse()
        
        # More info about httplib
        # http://docs.python.org/lib/module-httplib.html
        if response.status == 200:
            print import_succeed_message
            print get_response_message(response)
            print "Server response:%s\n" % response.read()
        else:
            print import_failed_message
            print get_response_message(response)
            #print "Server response:%s\n" % response.read()
            sender = get_username()
            send_email(get_mail_subject(sender, server, path), xml, sender, [diamonds_mail_box], "latin-1", mail_server_address)
            print mail_sent_message
        
        connection.close()
           
    else:
        print 'Sending only mail'
        sender = get_username()
        send_email(get_mail_subject(sender, server, path), xml, sender, [mail_address], "latin-1", mail_server_address)
    
    print "------------------------"
    print "Processed in %s" % get_process_time(time.time()-start_time)
    print "------------------------"

if __name__ == "__main__":
    main()
