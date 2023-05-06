/*

分布式：一个业务分拆多个子业务，部署在不同的服务器上
集群：同一个业务，部署在多个服务器上

用户请求访问图片时，则由CDN厂商提供智能DNS解析，将最近的（当然也可能有其它更复杂的策略，例如负载情况、健康状态等）服务节点地址返回给用户，
用户请求到达指定的服务器节点上，该节点上提供了类似Squid/Vanish的代理缓存服务，如果是第一次请求该路径，则会从源站获取图片资源返回客户端浏览器，
如果缓存中存在，则直接从缓存中获取并返回给客户端浏览器，完成请求/响应过程。





HTTP请求格式

当浏览器向Web服务器发出请求时，它向服务器传递了一个数据块，也就是请求信息，HTTP请求信息由3部分组成：

l 请求行:请求方法 URI协议 版本

l 请求头(Request Header)

l 请求正文

下面是一个HTTP请求的例子：

GET /sample.jsp HTTP/1.1

 

Accept:image/gif.image/jpeg,**

Accept-Language:zh-cn

Connection:Keep-Alive

Host:localhost

User-Agent:Mozila/4.0(compatible:MSIE5.01:Windows NT5.0)

Accept-Encoding:gzip,deflate.

 

（3）请求正文

请求头和请求正文之间是一个空行，这个行非常重要，它表示请求头已经结束，接下来的是请求正文。请求正文中可以包含客户提交的查询字符串信息：

username=jinqiao&password=1234

在以上的例子的HTTP请求中，请求的正文只有一行内容。当然，在实际应用中，HTTP请求正文可以包含更多的内容。

HTTP请求方法我这里只讨论GET方法与POST方法

l GET方法

GET方法是默认的HTTP请求方法，我们日常用GET方法来提交表单数据，然而用GET方法提交的表单数据只经过了简单的编码，同时它将作为URL的一部分向Web服务器发送，因此，如果使用GET方法来提交表单数据就存在着安全隐患上。例如

Http://127.0.0.1/login.jsp?Name=zhangshi&Age=30&Submit=?%E+??

从上面的URL请求中，很容易就可以辩认出表单提交的内容。（？之后的内容）另外由于GET方法提交的数据是作为URL请求的一部分所以提交的数据量不能太大

 

l POST方法

POST方法是GET方法的一个替代方法，它主要是向Web服务器提交表单数据，尤其是大批量的数据。POST方法克服了GET方法的一些缺点。通过POST方法提交表单数据时，数据不是作为URL请求的一部分而是作为标准数据传送给Web服务器，这就克服了GET方法中的信息无法保密和数据量太小的缺点。因此，出于安全的考虑以及对用户隐私的尊重，通常表单提交时采用POST方法。

　　从编程的角度来讲，如果用户通过GET方法提交数据，则数据存放在QUERY＿STRING环境变量中，而POST方法提交的数据则可以从标准输入流中获取。

 

http响应格式

HTTP应答与HTTP请求相似，HTTP响应也由3个部分构成，分别是：

l 　状态行

l 　响应头(Response Header)

l 　响应正文

在接收和解释请求消息后，服务器会返回一个HTTP响应消息。

状态行由协议版本、数字形式的状态代码、及相应的状态描述，各元素之间以空格分隔。

格式: HTTP-Version Status-Code Reason-Phrase CRLF

例如: HTTP/1.1 200 OK \r\n

 

状态代码：

状态代码由3位数字组成，表示请求是否被理解或被满足。

状态描述：

状态描述给出了关于状态代码的简短的文字描述。

状态代码的第一个数字定义了响应的类别，后面两位没有具体的分类。

第一个数字有五种可能的取值：

- 1xx: 指示信息―表示请求已接收，继续处理。

- 2xx: 成功―表示请求已经被成功接收、理解、接受。

- 3xx: 重定向―要完成请求必须进行更进一步的操作。

- 4xx: 客户端错误―请求有语法错误或请求无法实现。

- 5xx: 服务器端错误―服务器未能实现合法的请求。

状态代码状态描述 说明

200 OK 客户端请求成功

400 Bad Request 由于客户端请求有语法错误，不能被服务器所理解。

401 Unauthonzed 请求未经授权。这个状态代码必须和WWW-Authenticate报头域一起使用

403 Forbidden 服务器收到请求，但是拒绝提供服务。服务器通常会在响应正文中给出不提供服务的原因

404 Not Found 请求的资源不存在，例如，输入了错误的URL。

500 Internal Server Error 服务器发生不可预期的错误，导致无法完成客户端的请求。

503 Service Unavailable 服务器当前不能够处理客户端的请求，在一段时间之后，服务器可能会恢复正常。

 

响应头

响应头可能包括：

Location：

Location响应报头域用于重定向接受者到一个新的位置。例如：客户端所请求的页面已不存在原先的位置，为了让客户端重定向到这个页面新的位置，服务器端可以发回Location响应报头后使用重定向语句，让客户端去访问新的域名所对应的服务器上的资源。当我们在JSP中使用重定向语句的时候，服务器端向客户端发回的响应报头中，就会有Location响应报头域。

Server：

Server响应报头域包含了服务器用来处理请求的软件信息。它和User-Agent请求报头域是相对应的，前者发送服务器端软件的信息，后者发送客户端软件(浏览器)和操作系统的信息。下面是Server响应报头域的一个例子：Server: Apache-Coyote/1.1

WWW-Authenticate：

WWW-Authenticate响应报头域必须被包含在401(未授权的)响应消息中，这个报头域和前面讲到的Authorization请求报头域是相关的，当客户端收到401响应消息，就要决定是否请求服务器对其进行验证。如果要求服务器对其进行验证，就可以发送一个包含了 Authorization报头域的请求，下面是WWW-Authenticate响应报头域的一个例子：WWW-Authenticate: Basic realm="Basic Auth Test!"

从这个响应报头域，可以知道服务器端对我们所请求的资源采用的是基本验证机制。

Content-Encoding：

Content-Encoding实体报头域被使用作媒体类型的修饰符，它的值指示了已经被应用到实体正文的附加内容编码，因而要获得Content- Type报头域中所引用的媒体类型，必须采用相应的解码机制。Content-Encoding主要用语记录文档的压缩方法，下面是它的一个例子： Content-Encoding: gzip。如果一个实体正文采用了编码方式存储，在使用之前就必须进行解码。

Content-Language：

Content-Language实体报头域描述了资源所用的自然语言。Content-Language允许用户遵照自身的首选语言来识别和区分实体。如果这个实体内容仅仅打算提供给丹麦的阅读者，那么可以按照如下的方式设置这个实体报头域：Content-Language: da。

如果没有指定Content-Language报头域，那么实体内容将提供给所以语言的阅读者。

Content-Length：

Content-Length实体报头域用于指明正文的长度，以字节方式存储的十进制数字来表示，也就是一个数字字符占一个字节，用其对应的ASCII码存储传输。

要注意的是：这个长度仅仅是表示实体正文的长度，没有包括实体报头的长度。

Content-Type

Content-Type实体报头域用语指明发送给接收者的实体正文的媒体类型。例如：

Content-Type: text/html;charset=ISO-8859-1

Content-Type: text/html;charset=GB2312

Last-Modified

Last-Modified实体报头域用于指示资源最后的修改日期及时间。

Expires

Expires实体报头域给出响应过期的日期和时间。通常，代理服务器或浏览器会缓存一些页面。当用户再次访问这些页面时，直接从缓存中加载并显示给用户，这样缩短了响应的时间，减少服务器的负载。为了让代理服务器或浏览器在一段时间后更新页面，我们可以使用Expires实体报头域指定页面过期的时间。当用户又一次访问页面时，如果Expires报头域给出的日期和时间比Date普通报头域给出的日期和时间要早(或相同)，那么代理服务器或浏览器就不会再使用缓存的页面而是从服务器上请求更新的页面。不过要注意，即使页面过期了，也并不意味着服务器上的原始资源在此时间之前或之后发生了改变。

Expires实体报头域使用的日期和时间必须是RFC 1123中的日期格式，例如：

Expires: Thu, 15 Sep 2005 16:00:00 GMT

HTTP1.1的客户端和缓存必须将其他非法的日期格式(也包括0)看作已过期。例如，为了让浏览器不要缓存页面，我们也可以利用Expires实体报头域，设置它的值为0，如下(JSP)：response.setDateHeader("Expires",0);

下面是一个HTTP响应的例子：

HTTP/1.1 200 OK

Server:Apache Tomcat/5.0.12
Date:Mon,6Oct2003 13:23:42 GMT




请求报文附注： 
    HTTP请求包括三部分：请求行(Request Line)，头部(Headers)和数据体(Body)。其中，请求行由请求方法(method)，请求网址Request-URI和协议 (Protocol)构成，而请求头包括多个属性，数据体则可以被认为是附加在请求之后的文本或二进制文件。 
    下面这个例子显示了一个HTTP请求的Header内容，这些数据是真正以网络HTTP协议从IE浏览器传递到Apache服务器上的。 
GET /qingdao.html HTTP/1.1 
Accept:text/html, * / * 
Accept-Language:zh-cn 
Accept-Encoding:gzip,deflate 
User-Agent:Mozilla/4.0(compatible;MSIE 5.01;Windows NT 5.0;DigExt) 
Host: www.6book.net 
Referer: http://www.6book.net/beijing.html 
Connection:Keep-Alive 

    这段程序使用了6个Header，还有一些Header没有出现。我们参考这个例子具体解释HTTP请求格式。 
1.HTTP请求行：请求行格式为Method Request-URI Protocol。在上面这个例子里，"GET / HTTP/1.1"是请求行。 
2.Accept:指浏览器或其他客户可以接爱的MIME文件格式。可以根据它判断并返回适当的文件格式。 
3.Accept-Charset：指出浏览器可以接受的字符编码。英文浏览器的默认值是ISO-8859-1. 
4.Accept-Language：指出浏览器可以接受的语言种类，如en或en-us，指英语。 
5.Accept-Encoding：指出浏览器可以接受的编码方式。编码方式不同于文件格式，它是为了压缩文件并加速文件传递速度。浏览器在接收到Web响应之后先解码，然后再检查文件格式。 
6.Authorization：当使用密码机制时用来标识浏览器。 
7.Cache-Control：设置关于请求被代理服务器存储的相关选项。一般用不到。 
8.Connection：用来告诉服务器是否可以维持固定的HTTP连接。HTTP/1.1使用Keep-Alive为默认值，这样，当浏览器需要多个文件时(比如一个HTML文件和相关的图形文件)，不需要每次都建立连接。 
9.Content-Type：用来表名request的内容类型。可以用HttpServletRequest的 getContentType()方法取得。 
10.Cookie：浏览器用这个属性向服务器发送Cookie。Cookie是在浏览器中寄存的小型数据体，它可以记载和服务器相关的用户信息，也可以用来实现会话功能。 
11.Expect：表时客户预期的响应状态。 
12.From：给出客户端HTTP请求负责人的email地址。 
13.Host：对应网址URL中的Web名称和端口号。 
14.If-Match：供PUT方法使用。 
15.If-Modified-Since：客户使用这个属性表明它只需要在指定日期之后更改过的网页。因为浏览器可以使用其存储的文件而不必从服务器请求，这样节省了Web资源。由于Servlet是动态生成的网页，一般不需要使用这个属性。 
16.If-None-Match：和If-Match相反的操作，供PUT方法使用。 
17.If-Unmodified-Since：和If-Match-Since相反。 
18.Pragma：这个属性只有一种值，即Pragma：no-cache,表明如果servlet充当代理服务器，即使其有已经存储的网页，也要将请求传递给目的服务器。 
19.Proxy-Authorization：代理服务器使用这个属性，一般用不到。 
20.Range：如果客户有部分网页，这个属性可以请求剩余部分。 
21.Referer：表明产生请求的网页URL。 
比如从网页/beijing.html中点击一个链接到网页/qingdao.html,在向服务器发送的GET /beijing.html中的请求中，Referer是http://www.6book.net/qingdao.html 。这个属性可以用来跟踪Web请求是从什么网站来的。 
22.Upgrage：客户通过这个属性设定可以使用与HTTP/1.1不同的协议。 
23.User-Agent：是客户浏览器名称。 
24.Via：用来记录Web请求经过的代理服务器或Web通道。 
25.Warning：用来由客户声明传递或存储(cache)错误。 





HTTP响应代码、http头部详细分析（一）| 所属分类：http 响应代码    一、HTTP响应码响应码由三位十进制数字组成，它们出现在由HTTP服务器发送的响应的第一行。 
     响应码分五种类型，由它们的第一位数字表示： 
1xx：信息，请求收到，继续处理 
2xx：成功，行为被成功地接受、理解和采纳 
3xx：重定向，为了完成请求，必须进一步执行的动作 
4xx：客户端错误，请求包含语法错误或者请求无法实现 
5xx：服务器错误，服务器不能实现一种明显无效的请求 
下表显示每个响应码及其含义： 
100 继续101 分组交换协200 OK201 被创建202 被采纳203 非授权信息204 无内容205 重置内容206 部分内容300 多选项301 永久地传送302 找到303 参见其他304 未改动305 使用代理307 暂时重定向400 错误请求401 未授权402 要求付费403 禁止404 未找到405 不允许的方法406 不被采纳407 要求代理授权408 请求超时409 冲突410 过期的411 要求的长度412 前提不成立413 请求实例太大414 请求URI太大415 不支持的媒体类型416 无法满足的请求范围417 失败的预期500 内部服务器错误501 未被使用502 网关错误503 不可用的服务504 网关超时505 HTTP版本未被支持 
    二、HTTP头标头标由主键/值对组成。它们描述客户端或者服务器的属性、被传输的资源以及应该实现连接。 
     四种不同类型的头标： 
1.通用头标：即可用于请求，也可用于响应，是作为一个整体而不是特定资源与事务相关联。 
2.请求头标：允许客户端传递关于自身的信息和希望的响应形式。 
3.响应头标：服务器和于传递自身信息的响应。 
4.实体头标：定义被传送资源的信息。即可用于请求，也可用于响应。 
头标格式：<name>:<value><CRLF> 
下表描述在HTTP/1.1中用到的头标 
Accept 定义客户端可以处理的媒体类型，按优先级排序；在一个以逗号为分隔的列表中，可以定义多种类型和使用通配符。例如：Accept: image/jpeg,image/png,* / *Accept-Charset 定义客户端可以处理的字符集，按优先级排序；在一个以逗号为分隔的列表中，可以定义多种类型和使用通配符。例如：Accept-Charset: iso-8859-1,*,utf-8 
Accept-Encoding 定义客户端可以理解的编码机制。例如：Accept-Encoding:gzip,compress 
Accept-Language 定义客户端乐于接受的自然语言列表。例如：Accept-Language: en,de 
Accept-Ranges 一个响应头标，它允许服务器指明：将在给定的偏移和长度处，为资源组成部分的接受请求。该头标的值被理解为请求范围的度量单位。例如Accept-Ranges: bytes或Accept-Ranges: none 
Age 允许服务器规定自服务器生成该响应以来所经过的时间长度，以秒为单位。该头标主要用于缓存响应。例如：Age: 30 
Allow 一个响应头标，它定义一个由位于请求URI中的次源所支持的HTTP方法列表。例如：Allow: GET,PUT 
aUTHORIZATION 一个响应头标，用于定义访问一种资源所必需的授权（域和被编码的用户ID与口令）。例如：Authorization: Basic YXV0aG9yOnBoaWw= 
Cache-Control 一个用于定义缓存指令的通用头标。例如：Cache-Control: max-age=30 
Connection 一个用于表明是否保存socket连接为开放的通用头标。例如：Connection: close或Connection: keep-alive 
Content-Base 一种定义基本URI的实体头标，为了在实体范围内解析相对URLs。如果没有定义Content-Base头标解析相对URLs，使用Content- Location URI（存在且绝对）或使用URI请求。例如：Content-Base:   http://www.myweb.com 
Content-Encoding 一种介质类型修饰符，标明一个实体是如何编码的。例如：Content-Encoding: zipContent-Language 用于指定在输入流中数据的自然语言类型。例如：Content-Language: en 
Content-Length 指定包含于请求或响应中数据的字节长度。例如：Content-Length:382 
Content-Location 指定包含于请求或响应中的资源定位（URI）。如果是一绝。对URL它也作为被解析实体的相对URL的出发点。例如：Content-Location:   http://www.myweb.com/news 
Content-MD5 实体的一种MD5摘要，用作校验和。发送方和接受方都计算MD5摘要，接受方将其计算的值与此头标中传递的值进行比较。例如：Content-MD5: <base64 of 128 MD5 digest> 
Content-Range 随部分实体一同发送；标明被插入字节的低位与高位字节偏移，也标明此实体的总长度。例如：Content-Range: 1001-2000/5000 
Contern-Type 标明发送或者接收的实体的MIME类型。例如：Content-Type: text/html 
Date 发送HTTP消息的日期。例如：Date: Mon,10PR 18:42:51 GMT 
ETag 一种实体头标，它向被发送的资源分派一个唯一的标识符。对于可以使用多种URL请求的资源，ETag可以用于确定实际被发送的资源是否为同一资源。例如：ETag: '208f-419e-30f8dc99' 
Expires 指定实体的有效期。例如：Expires: Mon,05 Dec 2008 12:00:00 GMT 
Form 一种请求头标，给定控制用户代理的人工用户的电子邮件地址。例如：From:   webmaster@myweb.com 
Host 被请求资源的主机名。对于使用HTTP/1.1的请求而言，此域是强制性的。例如：Host:   www.myweb.com 
If-Modified-Since 如果包含了GET请求，导致该请求条件性地依赖于资源上次修改日期。如果出现了此头标，并且自指定日期以来，此资源已被修改，应该反回一个304响应代 码。例如：If-Modified-Since: Mon,10PR 18:42:51 GMT 
If-Match 如果包含于一个请求，指定一个或者多个实体标记。只发送其ETag与列表中标记区配的资源。例如：If-Match: '208f-419e-308dc99' 
If-None-Match 如果包含一个请求，指定一个或者多个实体标记。资源的ETag不与列表中的任何一个条件匹配，操作才执行。例如：If-None-Match: '208f-419e-308dc99' 
If-Range 指定资源的一个实体标记，客户端已经拥有此资源的一个拷贝。必须与Range头标一同使用。如果此实体自上次被客户端检索以来，还不曾修改过，那么服务器 只发送指定的范围，否则它将发送整个资源。例如：Range: byte=0-499<CRLF>If-Range:'208f-419e-30f8dc99' 
If-Unmodified-Since 只有自指定的日期以来，被请求的实体还不曾被修改过，才会返回此实体。例如：If-Unmodified-Since:Mon,10PR 18:42:51 GMT 
Last-Modified 指定被请求资源上次被修改的日期和时间。例如：Last-Modified: Mon,10PR 18:42:51 GMT 
Location 对于一个已经移动的资源，用于重定向请求者至另一个位置。与状态编码302（暂时移动）或者301（永久性移动）配合使用。例如：Location:   http://www2.myweb.com/index.jsp 
Max-Forwards 一个用于TRACE方法的请求头标，以指定代理或网关的最大数目，该请求通过网关才得以路由。在通过请求传递之前，代理或网关应该减少此数目。例如：Max-Forwards: 3 
Pragma 一个通用头标，它发送实现相关的信息。例如：Pragma: no-cache 
Proxy-Authenticate 类似于WWW-Authenticate，便是有意请求只来自请求链（代理）的下一个服务器的认证。例如：Proxy-Authenticate: Basic realm-admin 
Proxy-Proxy-Authorization 类似于授权，但并非有意传递任何比在即时服务器链中更进一步的内容。例如：Proxy-Proxy-Authorization: Basic YXV0aG9yOnBoaWw= 
Public 列表显示服务器所支持的方法集。例如：Public: OPTIONS,MGET,MHEAD,GET,HEAD 
Range 指定一种度量单位和一个部分被请求资源的偏移范围。例如：Range: bytes=206-5513 
Refener 一种请求头标域，标明产生请求的初始资源。对于HTML表单，它包含此表单的Web页面的地址。例如：Refener:   http://www.myweb.com/news/search.html 
Retry-After 一种响应头标域，由服务器与状态编码503（无法提供服务）配合发送，以标明再次请求之前应该等待多长时间。此时间即可以是一种日期，也可以是一种秒单位。例如：Retry-After: 18 
Server 一种标明Web服务器软件及其版本号的头标。例如：Server: Apache/2.0.46(Win32) 
Transfer-Encoding 一种通用头标，标明对应被接受方反向的消息体实施变换的类型。例如：Transfer-Encoding: chunked 
Upgrade 允许服务器指定一种新的协议或者新的协议版本，与响应编码101（切换协议）配合使用。例如：Upgrade: HTTP/2.0 
User-Agent 定义用于产生请求的软件类型（典型的如Web浏览器）。例如：User-Agent: Mozilla/4.0(compatible; MSIE 5.5; Windows NT; DigExt) 
Vary 一个响应头标，用于表示使用服务器驱动的协商从可用的响应表示中选择响应实体。例如：Vary: *Via 一个包含所有中间主机和协议的通用头标，用于满足请求。例如：Via: 1.0 fred.com, 1.1 wilma.com 
Warning 用于提供关于响应状态补充信息的响应头标。例如：Warning: 99   www.myweb.com   Piano needs tuning 
www-Authenticate 一个提示用户代理提供用户名和口令的响应头标，与状态编码401（未授权）配合使用。响应一个授权头标。例如：www-Authenticate: Basic realm=zxm.mgmt 








最全的HTTP头部信息分析 
2011-11-03 本文行家：尉聊子
    
HTTP头部解释1.Accept：告诉WEB服务器自己接受什么介质类型，* /*表示任何类型，type/*表示该类型下的所有子类型，type/sub-type。2.Accept-Charset：浏览器申明自己接收的字符集Accept-Encoding：浏览器申明自己接收的编码方法，通常指定压缩方法，是否支持压缩，支持什么压缩方法（gzip，deflate）Accept-Language：：浏览器申明

 HTTP 头部解释

1. Accept：告诉WEB服务器自己接受什么介质类型，* /* 表示任何类型，type/* 表示该类型下的所有子类型，type/sub-type。
 
2. Accept-Charset：   浏览器申明自己接收的字符集
   Accept-Encoding：  浏览器申明自己接收的编码方法，通常指定压缩方法，是否支持压缩，支持什么压缩方法  （gzip，deflate）
   Accept-Language：：浏览器申明自己接收的语言语言跟字符集的区别：中文是语言，中文有多种字符集，比如big5，gb2312，gbk等等。
 
3. Accept-Ranges：WEB服务器表明自己是否接受获取其某个实体的一部分（比如文件的一部分）的请求。bytes：表示接受，none：表示不接受。
 
4. Age：当代理服务器用自己缓存的实体去响应请求时，用该头部表明该实体从产生到现在经过多长时间了。
 
5. Authorization：当客户端接收到来自WEB服务器的 WWW-Authenticate 响应时，该头部来回应自己的身份验证信息给WEB服务器。
 
6. Cache-Control：请求：no-cache（不要缓存的实体，要求现在从WEB服务器去取）
                                     max-age：（只接受 Age 值小于 max-age 值，并且没有过期的对象）
                                     max-stale：（可以接受过去的对象，但是过期时间必须小于 
                                                        max-stale 值）
                                     min-fresh：（接受其新鲜生命期大于其当前 Age 跟 min-fresh 值之和的
                                                        缓存对象）
                            响应：public(可以用 Cached 内容回应任何用户)
                                      private（只能用缓存内容回应先前请求该内容的那个用户）
                                      no-cache（可以缓存，但是只有在跟WEB服务器验证了其有效后，才能返回给客户端）
                                      max-age：（本响应包含的对象的过期时间）
                                      ALL:  no-store（不允许缓存）
 
7. Connection：请求：close（告诉WEB服务器或者代理服务器，在完成本次请求的响应
                                                  后，断开连接，不要等待本次连接的后续请求了）。
                                 keepalive（告诉WEB服务器或者代理服务器，在完成本次请求的
                                                         响应后，保持连接，等待本次连接的后续请求）。
                       响应：close（连接已经关闭）。
                                 keepalive（连接保持着，在等待本次连接的后续请求）。
   Keep-Alive：如果浏览器请求保持连接，则该头部表明希望 WEB 服务器保持
                      连接多长时间（秒）。
                      例如：Keep-Alive：300
 
8. Content-Encoding：WEB服务器表明自己使用了什么压缩方法（gzip，deflate）压缩响应中的对象。 
                                 例如：Content-Encoding：gzip                   
   Content-Language：WEB 服务器告诉浏览器自己响应的对象的语言。
   Content-Length：    WEB 服务器告诉浏览器自己响应的对象的长度。
                                例如：Content-Length: 26012
   Content-Range：    WEB 服务器表明该响应包含的部分对象为整个对象的哪个部分。
                                例如：Content-Range: bytes 21010-47021/47022
   Content-Type：      WEB 服务器告诉浏览器自己响应的对象的类型。
                                例如：Content-Type：application/xml
 
9. ETag：就是一个对象（比如URL）的标志值，就一个对象而言，比如一个 html 文件，
              如果被修改了，其 Etag 也会别修改， 所以，ETag 的作用跟 Last-Modified 的
              作用差不多，主要供 WEB 服务器 判断一个对象是否改变了。
              比如前一次请求某个 html 文件时，获得了其 ETag，当这次又请求这个文件时， 
              浏览器就会把先前获得的 ETag 值发送给  WEB 服务器，然后 WEB 服务器
              会把这个 ETag 跟该文件的当前 ETag 进行对比，然后就知道这个文件
              有没有改变了。
         
10. Expired：WEB服务器表明该实体将在什么时候过期，对于过期了的对象，只有在
             跟WEB服务器验证了其有效性后，才能用来响应客户请求。是 HTTP/1.0 的头部。
             例如：Expires：Sat, 23 May 2009 10:02:12 GMT
 
11. Host：客户端指定自己想访问的WEB服务器的域名/IP 地址和端口号。
                例如：Host：rss.sina.com.cn
 
12. If-Match：如果对象的 ETag 没有改变，其实也就意味著对象没有改变，才执行请求的动作。
    If-None-Match：如果对象的 ETag 改变了，其实也就意味著对象也改变了，才执行请求的动作。
 
13. If-Modified-Since：如果请求的对象在该头部指定的时间之后修改了，才执行请求
                       的动作（比如返回对象），否则返回代码304，告诉浏览器该对象没有修改。
                       例如：If-Modified-Since：Thu, 10 Apr 2008 09:14:42 GMT
    If-Unmodified-Since：如果请求的对象在该头部指定的时间之后没修改过，才执行请求的动作（比如返回对象）。
 
14. If-Range：浏览器告诉 WEB 服务器，如果我请求的对象没有改变，就把我缺少的部分
               给我，如果对象改变了，就把整个对象给我。 浏览器通过发送请求对象的 
               ETag 或者 自己所知道的最后修改时间给 WEB 服务器，让其判断对象是否
               改变了。总是跟 Range 头部一起使用。
 
15. Last-Modified：WEB 服务器认为对象的最后修改时间，比如文件的最后修改时间，
                   动态页面的最后产生时间等等。例如：Last-Modified：Tue, 06 May 2008 02:42:43 GMT
 
16. Location：WEB 服务器告诉浏览器，试图访问的对象已经被移到别的位置了，到该头部指定的位置去取。
                        例如：Location：http://i0.sinaimg.cn/dy/deco/2008/0528/sinahome_0803_ws_005_text_0.gif
 
17. Pramga：主要使用 Pramga: no-cache，相当于 Cache-Control： no-cache。例如：Pragma：no-cache
 
18. Proxy-Authenticate： 代理服务器响应浏览器，要求其提供代理身份验证信息。
      Proxy-Authorization：浏览器响应代理服务器的身份验证请求，提供自己的身份信息。
 
19. Range：浏览器（比如 Flashget 多线程下载时）告诉 WEB 服务器自己想取对象的哪部分。
                    例如：Range: bytes=1173546-
 
20. Referer：浏览器向 WEB 服务器表明自己是从哪个 网页/URL 获得/点击 当前请求中的网址/URL。
                   例如：Referer：http://www.sina.com/
 
21. Server: WEB 服务器表明自己是什么软件及版本等信息。例如：Server：Apache/2.0.61 (Unix)
 
22. User-Agent: 浏览器表明自己的身份（是哪种浏览器）。
                        例如：User-Agent：Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN;   
                                  rv:1.8.1.14) Gecko/20080404 Firefox/2.0.0.14
 
23. Transfer-Encoding: WEB 服务器表明自己对本响应消息体（不是消息体里面的对象）作了怎样的编码，比如是否分块（chunked）。
                       例如：Transfer-Encoding: chunked
 
24. Vary: WEB服务器用该头部的内容告诉 Cache 服务器，在什么条件下才能用本响应
                 所返回的对象响应后续的请求。
                 假如源WEB服务器在接到第一个请求消息时，其响应消息的头部为：
                 Content-Encoding: gzip; Vary: Content-Encoding  那么 Cache 服务器会分析后续
                 请求消息的头部，检查其 Accept-Encoding，是否跟先前响应的 Vary 头部值
                 一致，即是否使用相同的内容编码方法，这样就可以防止 Cache 服务器用自己
                 Cache 里面压缩后的实体响应给不具备解压能力的浏览器。
                 例如：Vary：Accept-Encoding
 
25. Via： 列出从客户端到 OCS 或者相反方向的响应经过了哪些代理服务器，他们用
        什么协议（和版本）发送的请求。当客户端请求到达第一个代理服务器时，该服务器会在自己发出的请求里面
        添加 Via 头部，并填上自己的相关信息，当下一个代理服务器 收到第一个代理服务器的请求时，会在自己发
        出的请求里面复制前一个代理服务器的请求的Via头部，并把自己的相关信息加到后面， 以此类推，当 OCS 
        收到最后一个代理服务器的请求时，检查 Via 头部，就知道该请求所经过的路由。
        例如：Via：1.0 236-81.D07071953.sina.com.cn:80 (squid/2.6.STABLE13)










理解HTTP消息头【很完整，例子也很丰富】 .
分类： 网站 2014-09-08 00:16 154人阅读 评论(0) 收藏 举报 
目录(?)[+]

理解HTTP消息头 二常见的HTTP返回码403 Access Forbidden404 Object not found401 Access Denied302 Object Moved500 Internal Server Error理解HTTP消息头 三理解HTTP消息头 四服务器返回的消息Content-TypeContent-DispositionContent-Type与Content-DispositionCache（一）初识HTTP消息头

但凡搞WEB开发的人都离不开HTTP（超文本传输协议），而要了解HTTP，除了HTML本身以外，还有一部分不可忽视的就是HTTP消息头。
做过Socket编程的人都知道，当我们设计一个通信协议时，“消息头/消息体”的分割方式是很常用的，消息头告诉对方这个消息是干什么的，消息体告诉对方怎么干。HTTP传输的消息也是这样规定的，每一个HTTP包都分为HTTP头和HTTP体两部分，后者是可选的，而前者是必须的。每当我们打开一个网页，在上面点击右键，选择“查看源文件”，这时看到的HTML代码就是HTTP的消息体，那么消息头又在哪呢？IE浏览器不让我们看到这部分，但我们可以通过截取数据包等方法看到它。
下面就来看一个简单的例子：
首先制作一个非常简单的网页，它的内容只有一行：
<html><body>hello world</body></html>
把它放到WEB服务器上，比如IIS，然后用IE浏览器请求这个页面（http://localhost:8080/simple.htm），当我们请求这个页面时，浏览器实际做了以下四项工作：
1 解析我们输入的地址，从中分解出协议名、主机名、端口、对象路径等部分，对于我们的这个地址，解析得到的结果如下：
协议名：http
主机名：localhost
端口：8080
对象路径：/simple.htm
2 把以上部分结合本机自己的信息，封装成一个HTTP请求数据包
3 使用TCP协议连接到主机的指定端口（localhost, 8080），并发送已封装好的数据包
4 等待服务器返回数据，并解析返回数据，最后显示出来
由截取到的数据包我们不难发现浏览器生成的HTTP数据包的内容如下：
GET /simple.htm HTTP/1.1<CR>
Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, * / *<CR>
Accept-Language: zh-cn<CR>
Accept-Encoding: gzip, deflate<CR>
User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)<CR>
Host: localhost:8080<CR>
Connection: Keep-Alive<CR>
<CR>
为了显示清楚我把所有的回车的地方都加上了“<CR>”，注意最后还有一个空行加一个回车，这个空行正是HTTP规定的消息头和消息体的分界线，第一个空行以下的内容就是消息体，这个请求数据包是没有消息体的。
消息的第一行“GET”表示我们所使用的HTTP动作，其他可能的还有“POST”等，GET的消息没有消息体，而POST消息是有消息体的，消息体的内容就是要POST的数据。后面/simple.htm就是我们要请求的对象，之后HTTP1.1表示使用的是HTTP1.1协议。
第二行表示我们所用的浏览器能接受的Content-type，三四两行则是语言和编码信息，第五行显示出本机的相关系信息，包括浏览器类型、操作系统信息等，很多网站可以显示出你所使用的浏览器和操作系统版本，就是因为可以从这里获取到这些信息。
第六行表示我们所请求的主机和端口，第七行表示使用Keep-Alive方式，即数据传递完并不立即关闭连接。
服务器接收到这样的数据包以后会根据其内容做相应的处理，例如查找有没有“/simple.htm”这个对象，如果有，根据服务器的设置来决定如何处理，如果是HTM，则不需要什么复杂的处理，直接返回其内容即可。但在直接返回之前，还需要加上HTTP消息头。
服务器发回的完整HTTP消息如下：
HTTP/1.1 200 OK<CR>
Server: Microsoft-IIS/5.1<CR>
X-Powered-By: ASP.NET<CR>
Date: Fri, 03 Mar 2006 06:34:03 GMT<CR>
Content-Type: text/html<CR>
Accept-Ranges: bytes<CR>
Last-Modified: Fri, 03 Mar 2006 06:33:18 GMT<CR>
ETag: "5ca4f75b8c3ec61:9ee"<CR>
Content-Length: 37<CR>
<CR>
<html><body>hello world</body></html>
同样，我用“<CR>”来表示回车。可以看到，这个消息也是用空行切分成消息头和消息体两部分，消息体的部分正是我们前面写好的HTML代码。
消息头第一行“HTTP/1.1”也是表示所使用的协议，后面的“200 OK”是HTTP返回代码，200就表示操作成功，还有其他常见的如404表示对象未找到，500表示服务器错误，403表示不能浏览目录等等。
第二行表示这个服务器使用的WEB服务器软件，这里是IIS 5.1。第三行是ASP.Net的一个附加提示，没什么实际用处。第四行是处理此请求的时间。第五行就是所返回的消息的content-type，浏览器会根据它来决定如何处理消息体里面的内容，例如这里是text/html，那么浏览器就会启用HTML解析器来处理它，如果是image/jpeg，那么就会使用JPEG的解码器来处理。
消息头最后一行“Content-Length”表示消息体的长度，从空行以后的内容算起，以字节为单位，浏览器接收到它所指定的字节数的内容以后就会认为这个消息已经被完整接收了。
 
 
 
 
理解HTTP消息头 （二）
常见的HTTP返回码
上一篇文章里我简要的说了说HTTP消息头的格式，注意到在服务器返回的HTTP消息头里有一个“HTTP/1.1 200 OK”，这里的200是HTTP规定的返回代码，表示请求已经被正常处理完成。浏览器通过这个返回代码就可以知道服务器对所发请求的处理情况是什么，每一种返回代码都有自己的含义。这里列举几种常见的返回码。
1 403 Access Forbidden
如果我们试图请求服务器上一个文件夹，而在WEB服务器上这个文件夹并没有允许对这个文件夹列目录的话，就会返回这个代码。一个完整的403回复可能是这样的：（IIS5.1）
HTTP/1.1 403 Access Forbidden
Server: Microsoft-IIS/5.1
Date: Mon, 06 Mar 2006 08:57:39 GMT
Connection: close
Content-Type: text/html
Content-Length: 172
 
<html><head><title>Directory Listing Denied</title></head>
<body><h1>Directory Listing Denied</h1>This Virtual Directory does not allow contents to be listed.</body></html>
2 404 Object not found
当我们请求的对象在服务器上并不存在时，就会给出这个返回代码，这可能也是最常见的错误代码了。IIS给出的404消息内容很长，除了消息头以外还有一个完整的说明“为什么会这样”的网页。APACHE服务器的404消息比较简短，如下：
HTTP/1.1 404 Not Found
Date: Mon, 06 Mar 2006 09:03:14 GMT
Server: Apache/2.0.55 (Unix) PHP/5.0.5
Content-Length: 291
Keep-Alive: timeout=15, max=100
Connection: Keep-Alive
Content-Type: text/html; charset=iso-8859-1
 
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
<html><head>
<title>404 Not Found</title>
</head><body>
<h1>Not Found</h1>
<p>The requested URL /notexist was not found on this server.</p>
<hr>
<address>Apache/2.0.55 (Unix) PHP/5.0.5 Server at localhost Port 8080</address>
</body></html>
也许你会问，无论是404还是200，都会在消息体内给出一个说明网页，那么对于客户端来说二者有什么区别呢？一个比较明显的区别在于200是成功请求，浏览器会记录下这个地址，以便下次再访问时可以自动提示该地址，而404是失败请求，浏览器只会显示出返回的页面内容，并不会记录此地址，要再次访问时还需要输入完整的地址。
3 401 Access Denied
当WEB服务器不允许匿名访问，而我们又没有提供正确的用户名/密码时，服务器就会给出这个返回代码。在IIS中，设置IIS的安全属性为不允许匿名访问（如下图），此时直接访问的话就会得到以下返回结果：

HTTP/1.1 401 Access Denied
Server: Microsoft-IIS/5.1
Date: Mon, 06 Mar 2006 09:15:55 GMT
WWW-Authenticate: Negotiate
WWW-Authenticate: NTLM
Connection: close
Content-Length: 3964
Content-Type: text/html
 
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<html dir=ltr>
……
此时浏览器上给出的提示如下图，让我们输入用户名和密码：

因返回信息中消息体较长，只取前面两行内容。注意，如果是用localhost来访问本机的IIS，因IE可以直接取得当前用户的身份，它会和服务器间直接进行协商，所以不会看到401提示。
当我们在输入了用户名和密码以后，服务器与客户端会再进行两次对话。首先客户端向服务器索取一个公钥，服务器端会返回一个公钥，二者都用BASE64编码，相应的消息如下（编码部分已经做了处理）：
GET / HTTP/1.1
Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, * / *
Accept-Language: zh-cn
Accept-Encoding: gzip, deflate
User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)
Host: 192.168.0.55:8080
Connection: Keep-Alive
Authorization: Negotiate ABCDEFG……
 
HTTP/1.1 401 Access Denied
Server: Microsoft-IIS/5.1
Date: Mon, 06 Mar 2006 09:20:53 GMT
WWW-Authenticate: Negotiate HIJKLMN……
Content-Length: 3715
Content-Type: text/html
 
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<html dir=ltr>
……
客户端拿到公钥之后使用公钥对用户名和密码进行加密码，然后把加密以后的结果重新发给服务器：
GET / HTTP/1.1
Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, * / *
Accept-Language: zh-cn
Accept-Encoding: gzip, deflate
User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)
Host: 192.168.0.55:8080
Connection: Keep-Alive
Authorization: Negotiate OPQRST……
这样，如果验证通过，服务器端就会把请求的内容发送过来了，也就是说禁止匿名访问的网站会经过三次请求才可以看到页面。但因为客户端浏览器已经缓存了公钥，用同一个浏览器窗口再次请求这个网站上的其它页面时就可以直接发送验证信息，从而一次交互就可以完成了。
4 302 Object Moved
用过ASP的人都知道ASP中页面重定向至少有Redirect和Transfer两种方法。二的区别在于Redirect是客户端重定向，而Transfer是服务器端重定向，那么它们具体是如何通过HTTP消息头实现的呢？
先来看一下Transfer的例子：
例如ASP文件1.asp只有一行
<% Server.Transfer "1.htm" %>
HTML文件1.htm也只有一行：
<p>this is 1.htm</p>
如果我们从浏览器里请求1.asp，发送的请求是：
GET /1.asp HTTP/1.1
Accept: * / *
Accept-Language: zh-cn
Accept-Encoding: gzip, deflate
User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)
Host: localhost:8080
Connection: Keep-Alive
Cookie: ASPSESSIONIDACCTRTTT=PKKDJOPBAKMAMBNANIPIFDAP
注意请求的文件确实是1.asp，而得到的回应则是：
HTTP/1.1 200 OK
Server: Microsoft-IIS/5.1
Date: Mon, 06 Mar 2006 12:52:44 GMT
X-Powered-By: ASP.NET
Content-Length: 20
Content-Type: text/html
Cache-control: private
 
<p>this is 1.htm</p>
不难看出，通过Server.Transfer语句服务器端已经做了页面重定向，而客户端对此一无所知，表面上看上去得到的就是1.asp的结果。
如果把1.asp的内容改为：
<% Response.Redirect "1.htm" %>
再次请求1.asp，发送的请求没有变化，得到的回应却变成了：
HTTP/1.1 302 Object moved
Server: Microsoft-IIS/5.1
Date: Mon, 06 Mar 2006 12:55:57 GMT
X-Powered-By: ASP.NET
Location: 1.htm
Content-Length: 121
Content-Type: text/html
Cache-control: private
 
<head><title>Object moved</title></head>
<body><h1>Object Moved</h1>This object may be found <a HREF="">here</a>.</body>
注意HTTP的返回代码由200变成了302，表示这是一个重定向消息，客户端需要根据消息头中Location字段的值重新发送请求，于是就有了下面一组对话：
GET /1.htm HTTP/1.1
Accept: * / *
Accept-Language: zh-cn
Accept-Encoding: gzip, deflate
User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)
Host: localhost:8080
Connection: Keep-Alive
If-Modified-Since: Thu, 02 Mar 2006 06:50:13 GMT
If-None-Match: "b224758ec53dc61:9f0"
Cookie: ASPSESSIONIDACCTRTTT=PKKDJOPBAKMAMBNANIPIFDAP

HTTP/1.1 200 OK
Server: Microsoft-IIS/5.1
X-Powered-By: ASP.NET
Date: Mon, 06 Mar 2006 12:55:57 GMT
Content-Type: text/html
Accept-Ranges: bytes
Last-Modified: Mon, 06 Mar 2006 12:52:32 GMT
ETag: "76d85bd51c41c61:9f0"
Content-Length: 20
 
<p>this is 1.htm</p>
很明显，两种重定向方式虽然看上去结果很像，但在实现原理上有很大的不同。
5 500 Internal Server Error
500号错误发生在服务器程序有错误的时候，例如，ASP程序为
<% if %>
显然这个程序并不完整，于是得到的结果为：
HTTP/1.1 500 Internal Server Error
Server: Microsoft-IIS/5.1
Date: Mon, 06 Mar 2006 12:58:55 GMT
X-Powered-By: ASP.NET
Content-Length: 4301
Content-Type: text/html
Expires: Mon, 06 Mar 2006 12:58:55 GMT
Set-Cookie: ASPSESSIONIDACCTRTTT=ALKDJOPBPPKNPCNOEPCNOOPD; path=/
Cache-control: private

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<html dir=ltr>
……
服务器发送了500号错误，并且后面通过HTML的方式说明了错误的原因。

 
 
理解HTTP消息头 （三）

（三） 客户端发送的内容
这一次主要来观察HTTP消息头中客户端的请求，从中找到一些有意思的内容。
 
1 HTTP_REFERER
写两个简单的网页：
a.htm：
<a href=b.htm>to page b</a>
b.htm：
haha
内容很简单，就是网页A中有一个到B的链接。把它们放到IIS上，并访问网页A，从中再点击到B的链接，于是看到了B页的“haha”。那么这两次请求有什么不同吗？观察它们所发送的HTTP消息头，最明显的区别就是访问B页时比访问A页时多了一行：
Referer: http://localhost/a.htm
这一行就表示，用户要访问的B页是从A页链接过来的。
服务器端要想取得这个值也是很容易的，以ASP为例，只需要写一句
<% =Request.ServerVariables("HTTP_REFERER") %>
就可以了。
一些网站通过HTTP_REFERER来做安全验证，判断用户是不是从允许的页面链接来的，而不是直接从浏览器上打URL或从其他页面链接过来，这样可以从一定程度上防止网页被做非法使用。但从上述原理来看，想要骗过服务器也并不困难，只要手工构造输入的HTTP消息头就可以了，其他常用的手段还有通过HOSTS文件伪造域名等。
除了超链接以外，还有其他几种方式会导致HTTP_REFERER信息被发送，如：
内联框架：<iframe src=b.asp></iframe>
框架集：<frameset><frame src=b.asp></frameset>
表单提交：<form action=b.asp><input type=submit></form>
SCRIPT引用：<script src=b.asp></script>
CSS引用：<link rel=stylesheet type=text/css href=b.asp>
XML数据岛：<xml src=b.asp></xml>
而以下形式不会发送HTTP_REFERER：
script转向：<script>location.href="b.asp"</script>
script开新窗口：<script>window.open("b.asp");</script>
META转向：<meta http-equiv="refresh" content="0;URL=b.asp">
引入图片：<img src=b.asp>
 
2 COOKIE
COOKIE是大家都非常熟悉的了，通过它可以在客户端保存用户状态，即使用户关闭浏览器也能继续保存。那么客户端与服务器端是如何交换COOKIE信息的呢？没错，也是通过HTTP消息头。
首先写一个简单的ASP网页：
<%
Dim i
i =  Request.Cookies("key")
Response.Write i
Response.Cookies("key") = "haha"
Response.Cookies("key").Expires = #2007-1-1#
%>
第一次访问此网页时，屏幕上一片白，第二次访问时，则会显示出“haha”。通过阅读程序不难发现，屏幕上显示的内容实际上是COOKIE的内容，而第一次访问时还没有设置COOKIE的值，所以不会有显示，第二次显示的是第一次设置的值。那么对应的HTTP消息头应该是什么样的呢？
第一次请求时没什么不同，略过
第一次返回时消息内容多了下面这一行：
Set-Cookie: key=haha; expires=Sun, 31-Dec-2006 16:00:00 GMT; path=/
很明显，key=haha表示键名为“key”的COOKIE的值为“haha”，后面是这则COOKIE的过期时间，因为我用的中文操作系统的时区是东八区，2007年1月1日0点对应的GMT时间就是2006年12月31日16点。
第二次再访问此网页时，发送的内容多了如下一行：
Cookie: key=haha
它的内容就是刚才设的COOKIE的内容。可见，客户端在从服务器端得到COOKIE值以后就保存在硬盘上，再次访问时就会把它发送到服务器。发送时并没有发送过期时间，因为服务器对过期时间并不关心，当COOKIE过期后浏览器就不会再发送它了。
如果使用IE6.0浏览器并且禁用COOKIE功能，可以发现服务器端的set-cookie还是有的，但客户端并不会接受它，也不会发送它。有些网站，特别是在线投票网站通过记录COOKIE防止用户重复投票，破解很简单，只要用IE6浏览器并禁用COOKIE就可以了。也有的网站通过COOKIE值为某值来判断用户是否合法，这种判断也非常容易通过手工构造HTTP消息头来欺骗，当然用HOSTS的方式也是可以欺骗的。
 
3 SESSION
HTTP协议本身是无状态的，服务器和客户端都不保证用户访问期间连接会一直保持，事实上保持连接是HTTP1.1才有的新内容，当客户端发送的消息头中有“Connection: Keep-Alive”时表示客户端浏览器支持保持连接的工作方式，但这个连接也会在一段时间没有请求后自动断开，以节省服务器资源。为了在服务器端维持用户状态，SESSION就被发明出来了，现在各主流的动态网页制做工具都支持SESSION，但支持的方式不完全相同，以下皆以ASP为例。
当用户请求一个ASP网页时，在返回的HTTP消息头中会有一行：
Set-Cookie: ASPSESSIONIDCSQCRTBS=KOIPGIMBCOCBFMOBENDCAKDP; path=/
服务器通过COOKIE的方式告诉客户端你的SESSIONID是多少，在这里是“KOIPGIMBCOCBFMOBENDCAKDP”，并且服务器上保留了和此SESSIONID相关的数据，当同一用户再次发送请求时，还会把这个COOKIE再发送回去，服务器端根据此ID找到此用户的数据，也就实现了服务器端用户状态的保存。所以我们用ASP编程时可以使用“session("name")=user”这样的方式保存用户信息。注意此COOKIE内容里并没有过期时间，这表示这是一个当关闭浏览器时立即过期的COOKIE，它不会被保存到硬盘上。这种工作方式比单纯用COOKIE的方式要安全很多，因为在客户端并没有什么能让我们修改和欺骗的值，唯一的信息就是SESSIONID，而这个ID在浏览器关闭时会立即失效，除非别人能在你浏览网站期间或关闭浏览器后很短时间内知道此ID的值，才能做一些欺骗活动。因为服务器端判断SESSION过期的方式并不是断开连接或关闭浏览器，而是通过用户手工结束SESSION或等待超时，当用户关闭浏览器后的一段时间里SESSION还没有超时，所以这时如果知道了刚才的SESSIONID，还是可以欺骗的。因此最安全的办法还是在离开网站之前手工结束SESSION，很多网站都提供“Logout”功能，它会通过设置SESSION中的值为已退出状态或让SESSION立即过期从而起到安全的目的。
SESSION和COOKIE的方式各有优缺点。SESSION的优点是比较安全，不容易被欺骗，缺点是过期时间短，如果用过在超过过期时间里没有向服务器发送任何信息，就会被认为超过过期了；COOKIE则相反，根据服务器端设置的超时时间，可以长时间保留信息，即使关机再开机也可能保留状态，而安全性自然大打折扣。很多网站都提供两种验证方式相结合，如果用户临时用这台电脑访问此访问则需要输入用户名和密码，不保存COOKIE；如果用户使用的是自己的个人电脑，则可以让网站在自己硬盘上保留COOKIE，以后访问时就不需要重新输入用户名和密码了。
 
4 POST
浏览器访问服务器常用的方式有GET和POST两种，GET方式只发送HTTP消息头，没有消息体，也就是除了要GET的基本信息之外不向服务器提供其他信息，网页表单（FROM）的默认提交方式就是用GET方式，它会把所有向服务器提交的信息都作为URL后面的参数，如a.asp?a=1&b=2这样的方式。而当要提交的数据量很大，或者所提交内容不希望别人直接看到时，应该使用POST方式。POST方式提交的数据是作为HTTP消息体存在的，例如，写一个网页表单：
<form method=post>
<input type=text name=text1>
<input type=submit>
</form>
访问此网页，并在表单中填入一个“haha”，然后提交，可以看到此次提交所发送的信息如下：
POST /form.asp HTTP/1.1
Accept:  * / *
Referer: http://localhost:8080/form.asp
Accept-Language: zh-cn
Content-Type: application/x-www-form-urlencoded
Accept-Encoding: gzip, deflate
User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0; InfoPath.1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)
Host: localhost:8080
Content-Length: 10
Connection: Keep-Alive
Cache-Control: no-cache
Cookie: key=haha; ASPSESSIONIDCSQCRTBS=LOIPGIMBLMNOGCOBOMPJBOKP
text1=haha
前面关键字从“GET”变为了“POST”，Content-Type变成了“application/x-www-form-urlencoded”，后面内容并无大变化，只是多了一行：Content-Length: 10，表示提交的内容的长度。空行后面是消息体，内容就是表单中所填的内容。注意此时发送的内容只是“Name=Value”的形式，表单上其他的信息不会被发送，所以想直接从服务器端取得list box中所有的list item是办不到的，除非在提交前用一段script把所有的item内容都连在一起放到一个隐含表单域中。
如果是用表单上传文件，情况就要复杂一些了，首先是表单声明中要加上一句话：enctype='multipart/form-data'，表示这个表单将提交多段数据，并用HTML：input type=file来声明一个文件提交域。
表单内容如下：
<form method=post enctype='multipart/form-data'>
<input type=text name=text1>
<input type=file name=file1>
<input type=submit>
</form>
我们为text1输入文字：hehe，为file1选择文件haha.txt，其内容为“ABCDEFG”，然后提交此表单。提交的完全信息为：
POST /form.asp HTTP/1.1
Accept: * / *
Referer: http://localhost:8080/form.asp
Accept-Language: zh-cn
Content-Type: multipart/form-data; boundary=---------------------------7d62bf2f9066c
Accept-Encoding: gzip, deflate
User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0; InfoPath.1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)
Host: localhost:8080
Content-Length: 337
Connection: Keep-Alive
Cache-Control: no-cache
Cookie: key=haha; ASPSESSIONIDCSQCRTBS=LOIPGIMBLMNOGCOBOMPJBOKP
-----------------------------7d62bf2f9066c
Content-Disposition: form-data; name="text1"
hehe
-----------------------------7d62bf2f9066c
Content-Disposition: form-data; name="file1"; filename="H:\Documents and Settings\Administrator\桌面\haha.txt"
Content-Type: text/plain
ABCDEFG
-----------------------------7d62bf2f9066c--
 
显然这个提交的信息要比前述的复杂很多。Content-Type变成了“multipart/form-data”，后面还多了一个boundary，此值是为了区分POST的内容的区段用的，只要在内容中遇到了此值，就表示下面要开始一个新的区段了，每个区段的内容相对独立。如果遇到的是此值后面连着两个减号，则表示全部内容到此结束。每个段也分为段头和段体两部分，用空行隔开，每段都有自己的类型和相关信息。如第一区段是text1的值，它的名称是“text1”，值为“hehe”。第二段是文件内容，段首里表明了此文件域的名称“file1”和此文件在用户磁盘上的位置，后面就是文件的内容。
如果我们想要自己写一个上传文件组件来接收HTML表单传送的文件数据，那么最核心的任务就是解析此数据包，从中取得需要的信息。
 


 
 
理解HTTP消息头 （四）
服务器返回的消息
服务器返回的HTTP消息也分为消息头和消息体两部分。前面连载的第二篇里已经介绍了返回消息中常见返回代码的含义。对于非正常的返回代码的处理比较简单，只要照着要求去做就好了，而对于正常的返回代码（200），其处理方式就多种多样了。

1 Content-Type
Content-Type是返回消息中非常重要的内容，它标识出这个返回内容的类型，其值为“主类型/子类型”的格式，例如最常见的就是text/html，它的意思是说返回的内容是文本类型，这个文本又是HTML格式的。原则上浏览器会根据Content-Type来决定如何显示返回的消息体内容。常见的内容类型有：
text/html HTML文本
image/jpeg JPG图片
image/gif GIF图片
application/xml XML文档
audio/x-mpegurl MP3文件列表，如果安装了Winamp，则可以直接把它当面M3U文件来打开
更多的内容类型可以在注册表“HKCR\MIME\Database\Content Type”下看到
对于IE6浏览器来说，如果Content-Type中的类型和实际的消息体类型不一致，那么它会根据内容中的类型来分析实际应该是什么类型，对于JPG、GIF等常用图片格式都可以正确的识别出来，而不管Content-Type中写的是什么。
如果Content-Type中指定的是浏览器可以直接打开的类型，那么浏览器就会直接打开其内容显示出来，如果是被关联到其它应用程序的类型，这时就要查找注册表中关于这种类型的注册情况，如果是允许直接打开而不需要询问的，就会直接调出这个关联的应用程序来打开这个文件，但如果是不允许直接打开的，就会询问是否打开。对于没有关联到任何应用程序的类型，IE浏览器不知道它该如何打开，此时IE6就会把它当成XML来尝试打开。
2 Content-Disposition
如果用AddHeader的方法在HTTP消息头中加入Content-Disposition段，并指定其值为“attachment”，那么无论这个文件是何类型，浏览器都会提示我们下载此文件，因为此时它认为后面的消息体是一个“附件”，不需要由浏览器来处理了。例如，在ASP.Net中写入如下语句：
Response.AddHeader("Content-Disposition: attachment");
请求此页面是得到的结果如：
HTTP/1.1 200 OK
Server: Microsoft-IIS/5.1
Date: Thu, 23 Mar 2006 07:54:53 GMT
Content-Disposition: attachment
Cache-Control: private
Content-Type: text/html; charset=utf-8
……
也就是说，通过AddHeader函数可以为HTTP消息头加入我们自定义的内容。使用这种方法可以强制让浏览器提示下载文件，即使这个文件是我们已知的类型，基于是HTML网页。如果想要让用户下载时提示一个默认的文件名，只需要在前面一句话后加上“filename=文件名”即可。例如：
Response.AddHeader("Content-Disposition: attachment; filename=mypage.htm");
3 Content-Type与Content-Disposition
如果把Content-Type和Content-Disposition结合在一起使用会怎么样呢？
打开一个网页时，浏览器会首先看是否有Content-Disposition: attachment这一项，如果有，无论Content-Type的值是什么，都会提示文件下载。
如果指定了filename，就会提示默认的文件名为此文件名。注意到在IE6中除了“保存”按扭外还有“打开”按扭，此时打开文件的类型是由在filename中指定的文件扩展名决定的，例如让filename=mypic.jpg，浏览器就会查找默认的图片查看器来打开此文件。
如果没有指定filename，那么浏览器就根据Content-Type中的类型来决定文件的类型，例如Content-Type类型为image/gif，那么就会去查找默认的看GIF图片的工具，并且设置此文件的名字为所请求的网页的主名（不带扩展名）加上对应于此文件类弄扩展名，例如请求的mypage.aspx，就会自动变成mypage.gif。如果并没有指定Content-Type值，那么就默认它为“text/html”，并且保存的文件名就是所请求的网页文件名。
但如果没有指定Content-Disposition，那么就和前面关于Content-Type中所讨论的情况是一样的了。

4 Cache
返回消息中的Cache用于指定网页缓存。我们经常可以看到这样的情况，打开一个网页时速度不快，但再次打开时就会快很多，原因是浏览器已经对此页面进行了缓存，那么在同一浏览器窗口中再次打开此页时不会重新从服务器端获取。网页的缓存是由HTTP消息头中的“Cache-control”来控制的，常见的取值有private、no-cache、max-age、must-revalidate等，默认为private。其作用根据不同的重新浏览方式分为以下几种情况：
（1） 打开新窗口
如果指定cache-control的值为private、no-cache、must-revalidate，那么打开新窗口访问时都会重新访问服务器。而如果指定了max-age值，那么在此值内的时间里就不会重新访问服务器，例如：
Cache-control: max-age=5
表示当访问此网页后的5秒内再次访问不会去服务器
（2） 在地址栏回车
如果值为private或must-revalidate（和网上说的不一样），则只有第一次访问时会访问服务器，以后就不再访问。如果值为no-cache，那么每次都会访问。如果值为max-age，则在过期之前不会重复访问。
（3） 按后退按扭
如果值为private、must-revalidate、max-age，则不会重访问，而如果为no-cache，则每次都重复访问
（4） 按刷新按扭
无论为何值，都会重复访问

当指定Cache-control值为“no-cache”时，访问此页面不会在Internet临时文章夹留下页面备份。
另外，通过指定“Expires”值也会影响到缓存。例如，指定Expires值为一个早已过去的时间，那么访问此网时若重复在地址栏按回车，那么每次都会重复访问：
Expires: Fri, 31 Dec 1999 16:00:00 GMT

在ASP中，可以通过Response对象的Expires、ExpiresAbsolute属性控制Expires值；通过Response对象的CacheControl属性控制Cache-control的值，例如：
Response.ExpiresAbsolute = #2000-1-1# ' 指定绝对的过期时间，这个时间用的是服务器当地时间，会被自动转换为GMT时间
Response.Expires = 20  ' 指定相对的过期时间，以分钟为单位，表示从当前时间起过多少分钟过期。
Response.CacheControl = "no-cache" 

















HTTP协议:
引言
HTTP 是一个属于应用层的面向对象的协议，由于其简捷、快速的方式，适用于分布式超媒体信息系统。它于1990年提出，经过几年的使用与发
展，得到不断地完善和 扩展。目前在WWW中使用的是HTTP/1.0的第六版，HTTP/1.1的规范化工作正在进行之中，而且HTTP-NG(Next Generation
of HTTP)的建议已经提出。
HTTP协议的主要特点可概括如下：
1.支持客户/服务器模式。
2.简单快速：客户向服务器请求服务时，只需传送请求方法和路径。请求方法常用的有GET、HEAD、POST。每种方法规定了客户与服务器联系的类型不同。由于HTTP协议简单，使得HTTP服务器的程序规模小，因而通信速度很快。
3.灵活：HTTP允许传输任意类型的数据对象。正在传输的类型由Content-Type加以标记。
4.无连接：无连接的含义是限制每次连接只处理一个请求。服务器处理完客户的请求，并收到客户的应答后，即断开连接。采用这种方式可以节省传输时间。
5.无状态：HTTP协议是无状态协议。无状态是指协议对于事务处理没有记忆能力。缺少状态意味着如果后续处理需要前面的信息，则它必须重传，这样可能导致每次连接传送的数据量增大。另一方面，在服务器不需要先前信息时它的应答就较快。
 
一、HTTP协议详解之URL篇
http（超文本传输协议）是一个基于请求与响应模式的、无状态的、应用层的协议，常基于TCP的连接方式，HTTP1.1版本中给出一种持续连接的机制，绝大多数的Web开发，都是构建在HTTP协议之上的Web应用。
HTTP URL (URL是一种特殊类型的URI，包含了用于查找某个资源的足够的信息)的格式如下：
http://host[":"port][abs_path]
http表示要通过HTTP协议来定位网络资源；host表示合法的Internet主机域名或者IP地址；port指定一个端口号，为空则使用缺省端口 80；abs_path指定请求资源的URI；如果URL中没有给出abs_path，那么当它作为请求URI时，必须以"/"的形式给出，通常这个工作 浏览器自动帮我们完成。
eg:
1、输入：www.guet.edu.cn
浏览器自动转换成：http://www.guet.edu.cn/
2、http:192.168.0.116:8080/index.jsp 
 
二、HTTP协议详解之请求篇
    http请求由三部分组成，分别是：请求行、消息报头(又叫请求头)、请求正文
1、请求行以一个方法符号开头，以空格分开，后面跟着请求的URI和协议的版本，格式如下：Method Request-URI HTTP-Version CRLF  
其中 Method表示请求方法；Request-URI是一个统一资源标识符；HTTP-Version表示请求的HTTP协议版本；CRLF表示回车和换行（除了作为结
尾的CRLF外，不允许出现单独的CR或LF字符）。

请求方法（所有方法全为大写）有多种，各个方法的解释如下：
GET     请求获取Request-URI所标识的资源
POST    在Request-URI所标识的资源后附加新的数据
HEAD    请求获取由Request-URI所标识的资源的响应消息报头
PUT     请求服务器存储一个资源，并用Request-URI作为其标识
DELETE  请求服务器删除Request-URI所标识的资源
TRACE   请求服务器回送收到的请求信息，主要用于测试或诊断
CONNECT 保留将来使用
OPTIONS 请求查询服务器的性能，或者查询与资源相关的选项和需求
应用举例：
GET方法：在浏览器的地址栏中输入网址的方式访问网页时，浏览器采用GET方法向服务器获取资源，eg:GET /form.html HTTP/1.1 (CRLF)
POST方法要求被请求服务器接受附在请求后面的数据，常用于提交表单。
eg：POST /reg.jsp HTTP/ (CRLF)                                       请求行
Accept:image/gif,image/x-xbit,... (CRLF)                             下面到CRLF行的为请求头
...
HOST:www.guet.edu.cn (CRLF)
Content-Length:22 (CRLF)
Connection:Keep-Alive (CRLF)
Cache-Control:no-cache (CRLF)
(CRLF)         //该CRLF表示消息报头已经结束，在此之前为消息报头
user=jeffrey&pwd=1234  //此行以下为提交的数据                        请求正文(POST有请求正文，GET没有)
HEAD方法与GET方法几乎是一样的，对于HEAD请求的回应部分来 说，它的HTTP头部中包含的信息与通过GET请求所得到的信息是相同的。利用这个方法，不必传输整个资源内容，就可以得到Request-URI所标识 的资源的信息。该方法常用于测试超链接的有效性，是否可以访问，以及最近是否更新。
2、请求报头后述
3、请求正文(略) 
 
三、HTTP协议详解之响应篇
    在接收和解释请求消息后，服务器返回一个HTTP响应消息。
HTTP响应也是由三个部分组成，分别是：状态行(响应行)、消息报头(响应头部)、响应正文
1、状态行格式如下：
HTTP-Version Status-Code Reason-Phrase CRLF
其中，HTTP-Version表示服务器HTTP协议的版本；Status-Code表示服务器发回的响应状态代码；Reason-Phrase表示状态代码的文本描述。
状态代码有三位数字组成，第一个数字定义了响应的类别，且有五种可能取值：
1xx：指示信息--表示请求已接收，继续处理
2xx：成功--表示请求已被成功接收、理解、接受
3xx：重定向--要完成请求必须进行更进一步的操作
4xx：客户端错误--请求有语法错误或请求无法实现
5xx：服务器端错误--服务器未能实现合法的请求
常见状态代码、状态描述、说明：
200 OK      //客户端请求成功
400 Bad Request  //客户端请求有语法错误，不能被服务器所理解
401 Unauthorized //请求未经授权，这个状态代码必须和WWW-Authenticate报头域一起使用 
403 Forbidden  //服务器收到请求，但是拒绝提供服务
404 Not Found  //请求资源不存在，eg：输入了错误的URL
500 Internal Server Error //服务器发生不可预期的错误
503 Server Unavailable  //服务器当前不能处理客户端的请求，一段时间后可能恢复正常
eg：HTTP/1.1 200 OK （CRLF）
2、响应报头后述
3、响应正文就是服务器返回的资源的内容 
 
四、HTTP协议详解之消息报头篇
    HTTP消息由客户端到服务器的请求和服务器到客户端的响应组成。请求消息和响应消息都是由开始行（对于请求消息，开始行就是请求行，对于响应消息，开始行就是状态行），消息报头（可选），空行（只有CRLF的行），消息正文（可选）组成。
HTTP消息报头包括普通报头、请求报头、响应报头、实体报头。
每一个报头域都是由名字+"："+空格+值 组成，消息报头域的名字是大小写无关的。
1、普通报头
在普通报头中，有少数报头域用于所有的请求和响应消息，但并不用于被传输的实体，只用于传输的消息。
eg：
Cache-Control   用于指定缓存指令，缓存指令是单向的（响应中出现的缓存指令在请求中未必会出现），且是独立的（一个消息的缓存指令不会影响另一个消息处理的缓存机制），HTTP1.0使用的类似的报头域为Pragma。
请求时的缓存指令包括：no-cache（用于指示请求或响应消息不能缓存）、no-store、max-age、max-stale、min-fresh、only-if-cached;
响应时的缓存指令包括：public、private、no-cache、no-store、no-transform、must-revalidate、proxy-revalidate、max-age、s-maxage.
eg：为了指示IE浏览器（客户端）不要缓存页面，服务器端的JSP程序可以编写如下：response.sehHeader("Cache-Control","no-cache");
//response.setHeader("Pragma","no-cache");作用相当于上述代码，通常两者//合用
这句代码将在发送的响应消息中设置普通报头域：Cache-Control:no-cache

Date普通报头域表示消息产生的日期和时间
Connection普通报头域允许发送指定连接的选项。例如指定连接是连续，或者指定"close"选项，通知服务器，在响应完成后，关闭连接
2、请求报头
请求报头允许客户端向服务器端传递请求的附加信息以及客户端自身的信息。
常用的请求报头
Accept
Accept请求报头域用于指定客户端接受哪些类型的信息。eg：Accept：image/gif，表明客户端希望接受GIF图象格式的资源；Accept：text/html，表明客户端希望接受html文本。
Accept-Charset
Accept-Charset请求报头域用于指定客户端接受的字符集。eg：Accept-Charset:iso-8859-1,gb2312.如果在请求消息中没有设置这个域，缺省是任何字符集都可以接受。
Accept-Encoding
Accept-Encoding请求报头域类似于Accept，但是它是用于指定可接受的内容编码。eg：Accept-Encoding:gzip.deflate.如果请求消息中没有设置这个域服务器假定客户端对各种内容编码都可以接受。
Accept-Language
Accept-Language请求报头域类似于Accept，但是它是用于指定一种自然语言。eg：Accept-Language:zh-cn.如果请求消息中没有设置这个报头域，服务器假定客户端对各种语言都可以接受。
Authorization
Authorization请求报头域主要用于证明客户端有权查看某个资源。当浏览器访问一个页面时，如果收到服务器的响应代码为401（未授权），可以发送一个包含Authorization请求报头域的请求，要求服务器对其进行验证。
Host（发送请求时，该报头域是必需的）
Host请求报头域主要用于指定被请求资源的Internet主机和端口号，它通常从HTTP URL中提取出来的，eg：
我们在浏览器中输入：http://www.guet.edu.cn/index.html
浏览器发送的请求消息中，就会包含Host请求报头域，如下：
Host：www.guet.edu.cn
此处使用缺省端口号80，若指定了端口号，则变成：Host：www.guet.edu.cn:指定端口号
User-Agent
我们上网登陆论坛的时候，往往会看到一些欢迎信息，其中列出了你的操作系统的名称和版本，你所使用的浏览器的名称和版本，这往往让很多人感到很神奇，实际 上，服务器应用程序就是从User-Agent这个请求报头域中获取到这些信息。User-Agent请求报头域允许客户端将它的操作系统、浏览器和其它 属性告诉服务器。不过，这个报头域不是必需的，如果我们自己编写一个浏览器，不使用User-Agent请求报头域，那么服务器端就无法得知我们的信息 了。
请求报头举例：
GET /form.html HTTP/1.1 (CRLF)
Accept:image/gif,image/x-xbitmap,image/jpeg,application/x-shockwave-flash,application/vnd.ms-excel,application/vnd.ms-powerpoint,application/msword,* / * (CRLF)
Accept-Language:zh-cn (CRLF)
Accept-Encoding:gzip,deflate (CRLF)
If-Modified-Since:Wed,05 Jan 2007 11:21:25 GMT (CRLF)
If-None-Match:W/"80b1a4c018f3c41:8317" (CRLF)
User-Agent:Mozilla/4.0(compatible;MSIE6.0;Windows NT 5.0) (CRLF)
Host:www.guet.edu.cn (CRLF)
Connection:Keep-Alive (CRLF)
(CRLF)
3、响应报头
响应报头允许服务器传递不能放在状态行中的附加响应信息，以及关于服务器的信息和对Request-URI所标识的资源进行下一步访问的信息。
常用的响应报头
Location
Location响应报头域用于重定向接受者到一个新的位置。Location响应报头域常用在更换域名的时候。
Server
Server响应报头域包含了服务器用来处理请求的软件信息。与User-Agent请求报头域是相对应的。下面是
Server响应报头域的一个例子：
Server：Apache-Coyote/1.1
WWW-Authenticate
WWW-Authenticate响应报头域必须被包含在401（未授权的）响应消息中，客户端收到401响应消息时候，并发送Authorization报头域请求服务器对其进行验证时，服务端响应报头就包含该报头域。
eg：WWW-Authenticate:Basic realm="Basic Auth Test!"  //可以看出服务器对请求资源采用的是基本验证机制。

4、实体报头
请求和响应消息都可以传送一个实体。一个实体由实体报头域和实体正文组成，但并不是说实体报头域和实体正文要在一起发送，可以只发送实体报头域。实体报头定义了关于实体正文（eg：有无实体正文）和请求所标识的资源的元信息。
常用的实体报头
Content-Encoding
Content-Encoding实体报头域被用作媒体类型的修饰符，它的值指示了已经被应用到实体正文的附加内容的编码，因而要获得Content- Type报头域中所引用的媒体类型，必须采用相应的解码机制。Content-Encoding这样用于记录文档的压缩方法，eg：Content- Encoding：gzip
Content-Language
Content-Language实体报头域描述了资源所用的自然语言。没有设置该域则认为实体内容将提供给所有的语言阅读
者。eg：Content-Language:da
Content-Length
Content-Length实体报头域用于指明实体正文的长度，以字节方式存储的十进制数字来表示。
Content-Type
Content-Type实体报头域用语指明发送给接收者的实体正文的媒体类型。eg：
Content-Type:text/html;charset=ISO-8859-1
Content-Type:text/html;charset=GB2312
Last-Modified
Last-Modified实体报头域用于指示资源的最后修改日期和时间。
Expires
Expires实体报头域给出响应过期的日期和时间。为了让代理服务器或浏览器在一段时间以后更新缓存中(再次访问曾访问过的页面时，直接从缓存中加载， 缩短响应时间和降低服务器负载)的页面，我们可以使用Expires实体报头域指定页面过期的时间。eg：Expires：Thu，15 Sep 2006 16:23:12 GMT
HTTP1.1的客户端和缓存必须将其他非法的日期格式（包括0）看作已经过期。eg：为了让浏览器不要缓存页面，我们也可以利用Expires实体报头域，设置为0，jsp中程序如下：response.setDateHeader("Expires","0");
 
五、利用telnet观察http协议的通讯过程
    实验目的及原理：
    利用MS的telnet工具，通过手动输入http请求信息的方式，向服务器发出请求，服务器接收、解释和接受请求后，会返回一个响应，该响应会在telnet窗口上显示出来，从而从感性上加深对http协议的通讯过程的认识。
    实验步骤：
1、打开telnet
1.1 打开telnet
运行-->cmd-->telnet
1.2 打开telnet回显功能
set localecho
2、连接服务器并发送请求
2.1 open www.guet.edu.cn 80  //注意端口号不能省略
    HEAD /index.asp HTTP/1.0
    Host:www.guet.edu.cn
    
   /*我们可以变换请求方法,请求桂林电子主页内容,输入消息如下 
    open www.guet.edu.cn 80 
   
    GET /index.asp HTTP/1.0  //请求资源的内容
    Host:www.guet.edu.cn  
2.2 open www.sina.com.cn 80  //在命令提示符号下直接输入telnet www.sina.com.cn 80
    HEAD /index.asp HTTP/1.0
    Host:www.sina.com.cn
 
3 实验结果：
3.1 请求信息2.1得到的响应是:
HTTP/1.1 200 OK                                              //请求成功
Server: Microsoft-IIS/5.0                                    //web服务器
Date: Thu,08 Mar 200707:17:51 GMT
Connection: Keep-Alive                                 
Content-Length: 23330
Content-Type: text/html
Expries: Thu,08 Mar 2007 07:16:51 GMT
Set-Cookie: ASPSESSIONIDQAQBQQQB=BEJCDGKADEDJKLKKAJEOIMMH; path=/
Cache-control: private
//资源内容省略
3.2 请求信息2.2得到的响应是:
HTTP/1.0 404 Not Found       //请求失败
Date: Thu, 08 Mar 2007 07:50:50 GMT
Server: Apache/2.0.54 <Unix>
Last-Modified: Thu, 30 Nov 2006 11:35:41 GMT
ETag: "6277a-415-e7c76980"
Accept-Ranges: bytes
X-Powered-By: mod_xlayout_jh/0.0.1vhs.markII.remix
Vary: Accept-Encoding
Content-Type: text/html
X-Cache: MISS from zjm152-78.sina.com.cn
Via: 1.0 zjm152-78.sina.com.cn:80<squid/2.6.STABLES-20061207>
X-Cache: MISS from th-143.sina.com.cn
Connection: close

失去了跟主机的连接
按任意键继续...
4 .注意事项：1、出现输入错误，则请求不会成功。
          2、报头域不分大小写。
          3、更深一步了解HTTP协议，可以查看RFC2616，在http://www.letf.org/rfc上找到该文件。
          4、开发后台程序必须掌握http协议
六、HTTP协议相关技术补充
    1、基础：
    高层协议有：文件传输协议FTP、电子邮件传输协议SMTP、域名系统服务DNS、网络新闻传输协议NNTP和HTTP协议等
中介由三种：代理(Proxy)、网关(Gateway)和通道(Tunnel)，一个代理根据URI的绝对格式来接受请求，重写全部或部分消息，通过 URI的标识把已格式化过的请求发送到服务器。网关是一个接收代理，作为一些其它服务器的上层，并且如果必须的话，可以把请求翻译给下层的服务器协议。一 个通道作为不改变消息的两个连接之间的中继点。当通讯需要通过一个中介(例如：防火墙等)或者是中介不能识别消息的内容时，通道经常被使用。
     代理(Proxy)：一个中间程序，它可以充当一个服务器，也可以充当一个客户机，为其它客户机建立请求。请求是通过可能的翻译在内部或经过传递到其它的 服务器中。一个代理在发送请求信息之前，必须解释并且如果可能重写它。代理经常作为通过防火墙的客户机端的门户，代理还可以作为一个帮助应用来通过协议处 理没有被用户代理完成的请求。
网关(Gateway)：一个作为其它服务器中间媒介的服务器。与代理不同的是，网关接受请求就好象对被请求的资源来说它就是源服务器；发出请求的客户机并没有意识到它在同网关打交道。
网关经常作为通过防火墙的服务器端的门户，网关还可以作为一个协议翻译器以便存取那些存储在非HTTP系统中的资源。
    通道(Tunnel)：是作为两个连接中继的中介程序。一旦激活，通道便被认为不属于HTTP通讯，尽管通道可能是被一个HTTP请求初始化的。当被中继 的连接两端关闭时，通道便消失。当一个门户(Portal)必须存在或中介(Intermediary)不能解释中继的通讯时通道被经常使用。

2、协议分析的优势-HTTP分析器检测网络攻击
以模块化的方式对高层协议进行分析处理，将是未来入侵检测的方向。
HTTP及其代理的常用端口80、3128和8080在network部分用port标签进行了规定

3、HTTP协议Content Lenth限制漏洞导致拒绝服务攻击
使用POST方法时，可以设置ContentLenth来定义需要传送的数据长度，例如ContentLenth:999999999，在传送完成前，内 存不会释放，攻击者可以利用这个缺陷，连续向WEB服务器发送垃圾数据直至WEB服务器内存耗尽。这种攻击方法基本不会留下痕迹。
http://www.cnpaf.net/Class/HTTP/0532918532667330.html

4、利用HTTP协议的特性进行拒绝服务攻击的一些构思
服务器端忙于处理攻击者伪造的TCP连接请求而无暇理睬客户的正常请求（毕竟客户端的正常请求比率非常之小），此时从正常客户的角度看来，服务器失去响应，这种情况我们称作：服务器端受到了SYNFlood攻击（SYN洪水攻击）。
而Smurf、TearDrop等是利用ICMP报文来Flood和IP碎片攻击的。本文用"正常连接"的方法来产生拒绝服务攻击。
19端口在早期已经有人用来做Chargen攻击了，即Chargen_Denial_of_Service，但是！他们用的方法是在两台Chargen 服务器之间产生UDP连接，让服务器处理过多信息而DOWN掉，那么，干掉一台WEB服务器的条件就必须有2个：1.有Chargen服务2.有HTTP 服务
方法：攻击者伪造源IP给N台Chargen发送连接请求（Connect），Chargen接收到连接后就会返回每秒72字节的字符流（实际上根据网络实际情况，这个速度更快）给服务器。

5、Http指纹识别技术
   Http指纹识别的原理大致上也是相同的：记录不同服务器对Http协议执行中的微小差别进行识别.Http指纹识别比TCP/IP堆栈指纹识别复杂许 多,理由是定制Http服务器的配置文件、增加插件或组件使得更改Http的响应信息变的很容易,这样使得识别变的困难；然而定制TCP/IP堆栈的行为 需要对核心层进行修改,所以就容易识别.
      要让服务器返回不同的Banner信息的设置是很简单的,象Apache这样的开放源代码的Http服务器,用户可以在源代码里修改Banner信息,然 后重起Http服务就生效了；对于没有公开源代码的Http服务器比如微软的IIS或者是Netscape,可以在存放Banner信息的Dll文件中修 改,相关的文章有讨论的,这里不再赘述,当然这样的修改的效果还是不错的.另外一种模糊Banner信息的方法是使用插件。
常用测试请求：
1：HEAD/Http/1.0发送基本的Http请求
2：DELETE/Http/1.0发送那些不被允许的请求,比如Delete请求
3：GET/Http/3.0发送一个非法版本的Http协议请求
4：GET/JUNK/1.0发送一个不正确规格的Http协议请求
Http指纹识别工具Httprint,它通过运用统计学原理,组合模糊的逻辑学技术,能很有效的确定Http服务器的类型.它可以被用来收集和分析不同Http服务器产生的签名。

6、其他：为了提高用户使用浏览器时的性能，现代浏览器还支持并发的访问方式，浏览一个网页时同时建立多个连接，以迅速获得一个网页上的多个图标，这样能更快速完成整个网页的传输。
HTTP1.1中提供了这种持续连接的方式，而下一代HTTP协议：HTTP-NG更增加了有关会话控制、丰富的内容协商等方式的支持，来提供
更高效率的连接。
 
感谢冯・诺依曼先生.是他整出了世界上的第一台计算机,才使得我们这些后人鸟枪换炮,由"剪刀加糨糊"的"学术土匪"晋级为"鼠标加剪贴板"的"学术海盗".　　
      感谢负责答辩的老师.在我也不明白所写为何物的情况下,他们只问了我两个问题--都知道写的什么吗？知道；参考文献都看了么？看了.之后便让我通过了答辩.他们是如此和蔼可亲的老师,他们是如此善解人意的老师,他们是如此平易近人而又伟大的老师. 








为什么要对URI进行编码:

下载LOFTER客户端 
为什么需要Url编码，通常如果一样东西需要编码，说明这样东西并不适合传输。原因多种多样，如Size过大，包含隐私数据，对于Url来说，
之所 以要进行编码，是因为Url中有些字符会引起歧义。 例如Url参数字符串中使用key=value键值对这样的形式来传参，键值对之间以&符号分隔，
如/s?q=abc&ie=utf- 8。如果你的value字符串中包含了=或者&，那么势必会造成接收Url的服务器解析错误，因此必须将引起歧义的&和=符号进行转义， 
也就是对其进行编码。 又如，Url的编码格式采用的是ASCII码，而不是Unicode，这也就是说你不能在Url中包含任何非ASCII字符，例如中文。
否则如果客户端浏 览器和服务端浏览器支持的字符集不同的情况下，中文可能会造成问题。

Url编码的原则就是使用安全的字符（没有特殊用途或者特殊意义的可打印字符）去表示那些不安全的字符。 哪些字符需要编码 RFC3986文档规定，
Url中只允许包含英文字母（a-zA-Z）、数字（0-9）、-_.~4个特殊字符以及所有保留字符。 RFC3986文档对Url的编解码问题做出了详细的建议，
指出了哪些字符需要被编码才不会引起Url语义的转变，以及对为什么这些字符需要编码做出了相 应的解释。 US-ASCII字符集中没有对应的可打印
字符 Url中只允许使用可打印字符。US-ASCII码中的10-7F字节全都表示控制字符，这些字符都不能直接出现在Url中。同时，对于80-FF字节
（ISO-8859-1），由于已经超出了US-ACII定义的字节范围，因此也不可以放在Url中。

保留字符 Url可以划分成若干个组件，协议、主机、路径等。有一些字符（:/?#[]@）是用作分隔不同组件的。例如:冒号用于分隔协议和主机，
/用于分隔主机和 路径，?用于分隔路径和查询参数，等等。还有一些字符（!$&’()*+,;=）用于在每个组件中起到分隔作用的，如=用于表示
查询参数中的键值 对，&符号用于分隔查询多个键值对。当组件中的普通数据包含这些特殊字符时，需要对其进行编码。

RFC3986中指定了以下字符为保留字符： ! * ‘ ( ) ; : @ & = + $ , / ? # [ ] 不安全字符 还有一些字符，当他们直接放在Url中的时候，
可能会引起解析程序的歧义。这些字符被视为不安全字符，原因有很多。 空格 Url在传输的过程，或者用户在排版的过程，或者文本处理程序
在处理Url的过程，都有可能引入无关紧要的空格，或者将那些有意义的空格给去掉 引号以及<> 引号和尖括号通常用于在普通文本中起到分隔
Url的作用 # 通常用于表示书签或者锚点 % 百分号本身用作对不安全字符进行编码时使用的特殊字符，因此本身需要编码 {}|\^[]`~ 某一些
网关或者传输代理会篡改这些字符 

需要注意的是，对于Url中的合法字符，编码和不编码是等价的，但是对于上面提到的这些字符，如果不经过编码，那么它们有可能会造成Url语义
的不同。因 此对于Url而言，只有普通英文字符和数字，特殊字符$-_.+!*’()还有保留字符，才能出现在未经编码的Url之中。其他字符均需要
经过编码之后才 能出现在Url中。

Javascript中提供了3对函数用来对Url编码以得到合法的Url，它们分别是escape / unescape,encodeURI / decodeURI和encodeURIComponent / 
decodeURIComponent。由于解码和编码的过程是可逆的，因此这里只解释编码的过程。 这三个编码的函数