VIETNAM ONLINE — Tài Liệu Thiết Kế — Chương 31–35

**VIETNAM ONLINE**

*Thế Giới Du Lịch Ảo Toàn Quốc Việt Nam*

Tài liệu thiết kế bổ sung — Chương 31–35 — v1.0

*PRD · Vision & Design Pillars · Onboarding · Retention · Infrastructure*


# **31. Product Requirements Document (PRD)**
PRD là tài liệu định nghĩa chính xác Vietnam Online cần làm gì, cho ai, đến mức nào — trước khi bắt đầu dòng code đầu tiên. Đây là chuẩn duy nhất để team developer, designer, artist và investor cùng nhìn về một hướng.
## **31.1. Tóm Tắt Sản Phẩm (Product Summary)**

|**Trường**|**Nội dung**|
| :- | :- |
|Tên sản phẩm|Vietnam Online (tên nội bộ: VNO)|
|Thể loại|MMO Social Exploration / Virtual Tourism Game|
|Nền tảng mục tiêu|PC (Windows) → Android → iOS (theo thứ tự ưu tiên)|
|Mô hình doanh thu|Free-to-Play + Cosmetic Shop + Vietnam Pass + B2B Tourism Partnership|
|Ngôn ngữ ra mắt|Tiếng Việt, Tiếng Anh|
|Thị trường chính|Việt Nam (nội địa) + Diaspora Việt Nam toàn cầu + Du khách quốc tế|
|Rating độ tuổi|E (Everyone) — không bạo lực, không nội dung người lớn|
|Engine & Stack|Unreal Engine 5 + C++ | PostgreSQL + Redis | Go game server|
|MVP Target|1 thành phố (Hội An), 20–50 CCU, 20+ POI, multiplayer cơ bản|
|Phiên bản 1.0 Target|5 thành phố, 200 CCU/zone, đầy đủ economy & quest system|
## **31.2. Phát Biểu Vấn Đề (Problem Statement)**
Vietnam Online giải quyết 3 vấn đề cụ thể đang tồn tại:

- Rào cản du lịch thực tế: chi phí, thời gian, sức khỏe — nhiều người muốn khám phá Việt Nam nhưng không thể đến tận nơi
- Thiếu nền tảng số bảo tồn và phổ biến văn hóa Việt Nam một cách tương tác và hấp dẫn với thế hệ trẻ
- Ngành du lịch Việt Nam thiếu công cụ marketing sáng tạo để tiếp cận khách hàng trẻ tuổi toàn cầu trước khi họ quyết định điểm đến
## **31.3. User Personas (Chân Dung Người Dùng)**

|**Persona**|**Mô tả**|**Nhu cầu chính**|**Pain point**|
| :- | :- | :- | :- |
|Bạch Tuyết — SV 19 tuổi, Hà Nội|Sinh viên ĐH, thích khám phá nhưng không có tiền, dùng điện thoại 6h/ngày|Trải nghiệm du lịch ảo, gặp bạn mới, flex ảnh đẹp lên MXH|Game PC cần máy mạnh, không có tiền mua Pass cao cấp|
|Minh Khoa — Kỹ sư 28 tuổi, TP.HCM|Đi làm bận, thu nhập tốt, thích Việt Nam nhưng ít thời gian nghỉ|Khám phá nơi chưa từng đến, xả stress, lên kế hoạch trip thật|Không có giờ chơi dài, muốn session ngắn vẫn có giá trị|
|David Nguyen — Việt kiều 35 tuổi, Úc|Xa quê 10 năm, muốn kết nối lại văn hóa, dạy con về Việt Nam|Nostalgia, văn hóa chân thực, nói tiếng Việt với NPC|Lag cao (server xa), nội dung phải đúng văn hóa không sai lệch|
|Yuki — Sinh viên Nhật 22 tuổi, Tokyo|Mê văn hóa Đông Nam Á, theo dõi travel content về VN, chưa đến bao giờ|Khám phá VN từ góc nhìn người nước ngoài, học tiếng Việt cơ bản|Rào cản ngôn ngữ, cần UI bằng tiếng Anh hoặc Nhật đầy đủ|
|Cô Lan — Giáo viên 45 tuổi, Đà Nẵng|Dùng smartphone thành thạo, muốn dạy học sinh về địa lý Việt Nam qua game|Nội dung giáo dục chính xác, an toàn cho học sinh, dễ dùng|Lo ngại nội dung không phù hợp, giao diện phức tạp|
## **31.4. Functional Requirements (Yêu Cầu Chức Năng)**
### **31.4.1. Must Have — MVP (Giai đoạn 1)**

|**ID**|**Yêu cầu**|**Tiêu chí chấp nhận**|
| :- | :- | :- |
|FR-01|Đăng ký / Đăng nhập tài khoản|Email + password hoặc Google OAuth, xác thực email, JWT session|
|FR-02|Character creation cơ bản|Chọn giới tính, tone da, kiểu tóc, tên nhân vật — lưu vào DB|
|FR-03|Di chuyển tự do trong map|WASD + mouse, đi bộ và chạy, không xuyên tường, animation blend mượt|
|FR-04|Multiplayer 20–50 người|Thấy và nghe người chơi khác trong bán kính 100m, vị trí đồng bộ < 100ms|
|FR-05|Chat text proximity|Tin nhắn hiện trong bán kính 50m, lọc ngôn từ tự động, không lưu > 24h|
|FR-06|Minimap và navigation|Minimap phong cách bản đồ giấy, tên đường thật từ OSM, GPS marker|
|FR-07|POI system cơ bản|20+ địa điểm có tên, vị trí, mô tả ngắn, check-in, EXP reward|
|FR-08|Quest system cơ bản|10+ quest, accept/track/complete, reward VND game và EXP|
|FR-09|Thuê xe đạp và xe máy|Chọn xe, thanh toán VND game, trả xe, đổ xăng, vật lý di chuyển|
|FR-10|Save/Load trạng thái|Vị trí, inventory, quest, tiền — tự lưu mỗi 30 giây vào Supabase|
### **31.4.2. Should Have — Giai đoạn 2**
- FR-11: Hệ thống lưu trú (đặt phòng, check-in/out, 3+ loại hình)
- FR-12: Hệ thống ẩm thực (nhà hàng, gọi món, buff đặc sản)
- FR-13: NPC Layer 1 & 2 (lịch trình, dialog script, pathfinding)
- FR-14: Friend list và party system (tối đa 8 người)
- FR-15: Achievement system (20+ thành tựu đầu tiên)
- FR-16: Photo Mode và gallery trong game
- FR-17: Mở rộng 2–3 thành phố (Đà Nẵng hoặc Hà Nội)
### **31.4.3. Nice to Have — Giai đoạn 3+**
- FR-18: NPC Layer 3 với LLM AI hội thoại tự nhiên
- FR-19: Voice chat (proximity + party)
- FR-20: UGC platform đầy đủ (upload địa điểm, tour builder)
- FR-21: Mobile port (Android trước)
- FR-22: Hệ thống thời tiết thực từ API
- FR-23: Economy player-driven đầy đủ (photo market, tour marketplace)
## **31.5. Non-Functional Requirements (Yêu Cầu Phi Chức Năng)**

|**NFR**|**Yêu cầu**|**Mục tiêu đo lường**|
| :- | :- | :- |
|NFR-01 Performance|Client giữ ≥ 60 FPS trên GPU tầm trung|RTX 2060 / RX 5700 tại 1080p Ultra Settings|
|NFR-02 Latency|Server tick ≤ 100ms cho 95% request khi < 200 CCU/zone|Đo qua Prometheus, alert khi > 150ms|
|NFR-03 Uptime|Server availability ≥ 99.5% (downtime < 44h/năm)|Grafana dashboard, auto-restart on crash|
|NFR-04 Scalability|Hệ thống scale horizontal khi > 80% capacity|Auto-scaling trigger trên Oracle Cloud|
|NFR-05 Security|Không có critical vulnerability (OWASP Top 10)|Pentest trước mỗi major release|
|NFR-06 Load Time|Zone load time < 10 giây trên kết nối 4G Việt Nam (20 Mbps)|Đo trên thiết bị thực, không emulator|
|NFR-07 Data Safety|Backup database mỗi 1h, RTO < 4h, RPO < 1h|Test restore thực tế hàng tháng|
|NFR-08 Accessibility|Tuân thủ WCAG 2.1 AA cho toàn bộ UI|Audit tool + manual test với người dùng|
## **31.6. Out of Scope (Ngoài Phạm Vi)**
*Các tính năng sau KHÔNG nằm trong phạm vi Vietnam Online — để tránh scope creep làm chậm tiến độ:*

- Combat system (đánh nhau, bắn súng, PvP) — không phải game chiến đấu
- Crafting system phức tạp — chỉ mua/bán item, không tự chế tạo vũ khí
- Housing/Land ownership — không mua đất, mua nhà trong game (giai đoạn này)
- Blockchain/NFT integration — không áp dụng trong thiết kế hiện tại
- Lồng tiếng cho 100% NPC — chỉ Layer 3 có voice actor, Layer 2 dùng TTS
- VR support — không trong roadmap 24 tháng đầu
- Bản đồ thế giới ngoài Việt Nam — focus 100% Việt Nam trong ít nhất 3 năm đầu
## **31.7. Assumptions & Dependencies (Giả Định & Phụ Thuộc)**

|**Loại**|**Nội dung**|**Rủi ro nếu sai**|
| :- | :- | :- |
|Assumption|OpenStreetMap có đủ dữ liệu đường phố Việt Nam ở độ chính xác chấp nhận được|Phải bổ sung thủ công → tốn thêm 2-3 tháng|
|Assumption|Cesium for Unreal hoạt động ổn định trên UE 5.3+|Phải tìm giải pháp bản đồ thay thế|
|Assumption|LLM API cost giảm đủ để NPC AI Layer 3 khả thi trong ngân sách giai đoạn 4|Trì hoãn AI NPC hoặc dùng script cố định|
|Dependency|Supabase duy trì free tier đủ dùng cho giai đoạn 1-2|Phải chuyển sang paid plan sớm hơn dự kiến|
|Dependency|Epic Online Services miễn phí cho dự án indie|Phải xây authentication riêng hoặc dùng Firebase|
|Dependency|Đăng ký game tại Bộ TT&TT được chấp thuận trước soft launch|Trì hoãn launch tại Việt Nam|


# **32. Vision & Design Pillars — Tầm Nhìn & Trụ Cột Thiết Kế**
Mọi quyết định thiết kế trong Vietnam Online — từ màu của nút bấm đến cơ chế kinh tế — đều phải có thể trả lời câu hỏi: "Tại sao chúng ta làm thế này?" Chương này là câu trả lời duy nhất và nhất quán cho toàn bộ team.
## **32.1. Tầm Nhìn Sản Phẩm (Product Vision)**
***"Trở thành cánh cửa số dẫn mọi người đến Việt Nam — trước khi họ đặt chân đến thật."***

Vietnam Online không cạnh tranh với Google Maps hay game nhập vai phương Tây. Nó tạo ra thể loại riêng: Virtual Cultural Tourism — du lịch văn hóa ảo. Mục tiêu dài hạn là trở thành đại sứ kỹ thuật số của Việt Nam trên toàn thế giới.
## **32.2. Năm Trụ Cột Thiết Kế (Five Design Pillars)**
*Mọi feature mới đề xuất phải phục vụ ít nhất 1 trong 5 trụ cột dưới đây. Nếu không — từ chối đề xuất đó.*
### **Trụ cột 1 — AUTHENTICITY (Chân Thực)**
Vietnam Online phải cảm thấy như Việt Nam thật — không phải một Việt Nam lý tưởng hóa hay cường điệu. Đây là cam kết quan trọng nhất và cũng khó giữ nhất.

|**Áp dụng**|**Không áp dụng**|
| :- | :- |
|Tên đường, tên địa điểm lấy từ OSM thực tế|Thêm tòa nhà khổng lồ không có thật để 'trông đẹp hơn'|
|Giá dịch vụ tham chiếu giá thực ngoài đời|Làm mọi thứ rẻ hoặc đắt bất hợp lý để balance game|
|NPC nói đúng giọng địa phương (Bắc/Trung/Nam)|Tất cả NPC nói giọng trung hoặc giọng nước ngoài|
|Thời tiết đồng bộ thực tế theo vùng|Thời tiết chỉ là visual, không ảnh hưởng gameplay gì|
|Lễ hội đúng ngày âm lịch và phong tục|Lễ hội chỉ là skin đổi màu, không có chiều sâu văn hóa|
### **Trụ cột 2 — CONNECTION (Kết Nối)**
Khoảnh khắc đáng nhớ nhất không đến từ bản đồ đẹp — mà từ người khác. Thiết kế phải luôn tạo cơ hội để người chơi gặp nhau, giúp nhau, nhớ nhau.

- Mỗi mechanic mới: có thể làm tốt hơn / thú vị hơn khi chơi cùng người khác không?
- Không có nội dung nào buộc phải chơi một mình để hoàn thành
- Hệ thống reputation và review đặt nền tảng cho sự tin tưởng giữa người chơi
- NPC không thay thế người chơi — NPC bổ sung cho thế giới khi không có người chơi đủ đông
### **Trụ cột 3 — DISCOVERY (Khám Phá)**
Người chơi phải liên tục cảm thấy có điều gì đó mới để tìm thấy — dù đã chơi 1 giờ hay 1.000 giờ.

- 63 tỉnh thành = 63 thế giới khác nhau về văn hóa, ẩm thực, kiến trúc, con người
- Hidden locations, secret quests, NPC chỉ xuất hiện vào giờ nhất định tạo lớp khám phá sâu
- UGC từ cộng đồng liên tục bổ sung nội dung mới mà team phát triển không thể tự tạo hết
- Dynamic world (Chương 25) đảm bảo thế giới thay đổi dù người chơi đứng yên
### **Trụ cột 4 — RESPECT (Tôn Trọng)**
Tôn trọng 3 đối tượng: người chơi, văn hóa Việt Nam, và thời gian của người dùng.

|**Đối tượng**|**Biểu hiện trong game**|
| :- | :- |
|Tôn trọng người chơi|Không dark pattern, không push notification spam, không pay wall chặn content chính, không punish người không online mỗi ngày|
|Tôn trọng văn hóa|Fact-check mọi thông tin lịch sử, mời chuyên gia tư vấn nội dung nhạy cảm, không stereotype 54 dân tộc|
|Tôn trọng thời gian|Session 15 phút vẫn có giá trị, không có nội dung chỉ mở sau 100 giờ chơi, không grinding vô nghĩa|
### **Trụ cột 5 — SUSTAINABILITY (Bền Vững)**
Vietnam Online phải tồn tại được về mặt kinh tế để thực hiện sứ mệnh văn hóa. Một game đẹp nhưng phá sản sau 1 năm không giúp ích được gì.

- Mô hình F2P: người chơi free trải nghiệm đầy đủ — trả tiền để cá nhân hóa và hỗ trợ dự án
- B2B tourism partnership là nguồn thu dài hạn, không phụ thuộc vào số lượng giao dịch nhỏ lẻ
- Content được thiết kế để scale không tuyến tính: quy trình UGC và AI giúp nội dung tăng nhanh hơn chi phí
- Open source một số component (bản đồ engine, OSM tooling) để xây dựng community developer
## **32.3. Bộ Câu Hỏi Kiểm Tra Thiết Kế (Design Litmus Test)**
Trước khi implement bất kỳ tính năng nào, đặt 5 câu hỏi này:

|**#**|**Câu hỏi**|**Nếu câu trả lời là KHÔNG**|
| :- | :- | :- |
|Q1|Tính năng này có phục vụ ít nhất 1 trong 5 Design Pillar không?|Từ chối hoặc redesign|
|Q2|Người chơi miễn phí có trải nghiệm được tính năng này không?|Cần xem xét lại mô hình monetization|
|Q3|Tính năng này có phản ánh Việt Nam chân thực không?|Cần fact-check hoặc tư vấn chuyên gia|
|Q4|Tính năng này có thể được giải thích trong vòng 10 giây không?|Quá phức tạp — đơn giản hóa|
|Q5|Khi chơi cùng người khác, tính năng này có tốt hơn không?|Đánh giá lại — ưu tiên tính xã hội|
## **32.4. Anti-Patterns — Những Gì Vietnam Online KHÔNG Làm**

|**Anti-pattern**|**Mô tả**|**Tại sao tránh**|
| :- | :- | :- |
|FOMO Mechanics|Sự kiện giới hạn 24h, countdown timer gây áp lực mua ngay|Gây stress, làm người chơi cảm thấy bị ép buộc|
|Loot Box mờ ám|Hộp ngẫu nhiên không rõ tỷ lệ, teaser item xịn khó đạt|Vi phạm nguyên tắc minh bạch và luật pháp một số nước|
|Pay Wall Nội Dung|Địa điểm, zone, quest chỉ mở bằng tiền thật|Phá vỡ triết lý F2P và mất lòng tin cộng đồng|
|Grinding Nhàm Chán|Làm đi làm lại một việc 100 lần chỉ để unlock|Không phù hợp với game khám phá văn hóa|
|Dark UI Patterns|Nút Cancel nhỏ, đặt sai vị trí để dễ mua nhầm|Làm mất uy tín, có thể vi phạm quy định người tiêu dùng|
|Notification Spam|Push 5+ thông báo/ngày khi không chơi|Người dùng tắt thông báo hoặc gỡ app|


# **33. Onboarding Design — Thiết Kế Trải Nghiệm Người Mới**
Onboarding là khoảng khắc quan trọng nhất trong vòng đời người chơi. Nghiên cứu ngành game cho thấy 60-70% người chơi bỏ game trong 10 phút đầu tiên nếu không hiểu phải làm gì. Vietnam Online có 5 phút để tạo ấn tượng đầu tiên không thể xóa nhòa.
## **33.1. Onboarding Philosophy**
- "Show, don't tell" — không đọc wall of text, hãy để người chơi trải nghiệm ngay
- "Hand-hold khi cần, buông tay đúng lúc" — tutorial 5 phút, sau đó thế giới mở hoàn toàn
- "Reward sớm" — người chơi cảm thấy thành công trong 2 phút đầu tiên
- "Không bao giờ nói 'bạn sai rồi'" — hướng dẫn lại bằng gợi ý tích cực
- "Tutorial là story" — người chơi không nhận ra mình đang học, chỉ thấy mình đang du lịch
## **33.2. Onboarding Flow — Phút Từng Phút**

|**Thời điểm**|**Màn hình / Sự kiện**|**Mục tiêu học**|**Cảm xúc mục tiêu**|
| :- | :- | :- | :- |
|0:00 – 0:30|Logo → Cinematic ngắn 30 giây: flycam Hội An lúc bình minh, âm nhạc đàn tranh, không có text|Thiết lập bầu không khí, tạo mong đợi|Ngạc nhiên, tò mò, muốn khám phá|
|0:30 – 1:30|Character Creator: giao diện đơn giản, chỉ 3 lựa chọn ban đầu (giới tính, tone da, tên). Nút 'Tuỳ chỉnh thêm' cho người muốn chi tiết hơn|Tạo cảm giác sở hữu nhân vật|Hứng khởi, cá nhân hóa|
|1:30 – 2:00|Cutscene ngắn: nhân vật bước xuống xe khách, đặt chân lần đầu vào Hội An. NPC bên cạnh nói 'Chào mừng đến Hội An, bạn ơi!'|Contextualize — người chơi biết mình đang ở đâu và tại sao|Ấm áp, được chào đón|
|2:00 – 3:30|Tutorial di chuyển: tooltip nhỏ ở góc màn hình gợi ý WASD / joystick. NPC đi cùng, người chơi theo NPC đến một điểm gần (30 giây đi bộ). Không có thanh tiến trình tutorial|Học di chuyển một cách tự nhiên|Tự tin, kiểm soát được|
|3:30 – 4:30|Đến Chùa Cầu: NPC kể 1 câu chuyện ngắn về nơi này. Xuất hiện nút 'Check-in' nhấp nháy nhẹ. Người chơi nhấn → Animation check-in đẹp + nhận 200.000 VND game + EXP đầu tiên|Học check-in, nhận reward đầu tiên, cảm giác accomplish|Vui vẻ, được thưởng, muốn làm tiếp|
|4:30 – 5:30|Quest đầu tiên tự động pop: 'Chụp 3 bức ảnh quanh Phố Cổ'. Minimap sáng lên 3 điểm gợi ý. Người chơi tự do đi chụp — không có timer, không áp lực|Giới thiệu quest system và photo system, để người chơi tự khám phá|Tự do, thoải mái, muốn lang thang|
|5:30+|Tutorial kết thúc. World mở hoàn toàn. Popup nhỏ: 'Thế giới đang chờ bạn. Khám phá thôi!' với nút đóng|Transition từ tutorial sang exploration tự do|Phấn khích, muốn tiếp tục|
## **33.3. New Player Experience — FTUE (First Time User Experience)**
### **Ngày 1 — Hook**
- Mục tiêu: người chơi hoàn thành ít nhất 1 quest chính và check-in 3 địa điểm trong session đầu tiên
- Trigger gợi ý: nếu người chơi đứng yên > 60 giây, NPC nearby tiếp cận và hỏi 'Bạn cần giúp gì không?'
- End-of-session summary: khi đóng game, hiện 'Hôm nay bạn đã đến: X địa điểm, kiếm được Y VND, gặp Z người chơi' — tạo cảm giác hoàn thành
### **Ngày 2 — Habit Formation**
- Welcome back message khi login: NPC quen (từ ngày 1) nói 'Ô, bạn quay lại rồi! Hôm qua bạn có nhắc đến... [memory từ session trước]'
- Daily quest mới xuất hiện — tạo lý do để quay lại mỗi ngày
- Gợi ý tính năng mới: ngày 2 giới thiệu xe máy, ngày 3 giới thiệu nhà hàng, ngày 4 giới thiệu bạn bè — không dump tất cả ngày 1
### **Ngày 7 — Commitment**
- Milestone quest: 'Tuần đầu ở Hội An' — hoàn thành unlock cosmetic đặc biệt và title 'Người Mới Của Phố Cổ'
- Friend suggestion: nếu chưa có bạn bè, gợi ý 3 người chơi cùng zone để kết bạn
- First zone complete badge: nếu đã check-in đủ 10/20 POI → badge 'Người Khám Phá Hội An'
## **33.4. Contextual Help System**

|**Trigger**|**Hành động**|**Ví dụ**|
| :- | :- | :- |
|Người chơi đứng yên > 90 giây|NPC gần nhất tiếp cận với gợi ý nhẹ nhàng|'Bạn có muốn tôi giới thiệu vài nơi hay ho quanh đây không?'|
|Wallet về 0|Tooltip popup: 'Hết tiền rồi! Thử nhận nhiệm vụ từ NPC để kiếm thêm nhé'|Gợi ý quest NPC gần nhất có reward cao|
|Lần đầu tiên lên xe máy|Tutorial mini 15 giây: WASD để lái, E để dừng, xe máy không vào được hẻm nhỏ|Animation minh họa + thực hành ngay|
|Lần đầu gặp người chơi khác|Hint nhỏ: 'Đây là người chơi thật! Bạn có thể chat bằng phím T'|Hiện bong bóng chat mẫu|
|Inventory đầy|Toast notification: 'Ba lô đầy rồi! Dùng hoặc bán bớt đồ tại chợ gần đây'|Waypoint chợ xuất hiện trên minimap|
## **33.5. Onboarding Metrics & Optimization**

|**Metric**|**Target**|**Hành động nếu dưới target**|
| :- | :- | :- |
|Tutorial completion rate|> 85% hoàn thành toàn bộ 5 phút tutorial|Rút ngắn hoặc đơn giản hóa bước có drop-off cao nhất|
|D1 Retention|> 45% quay lại ngày 2|Tăng end-of-session reward, cải thiện hook ngày 1|
|Time to First Quest Complete|< 8 phút kể từ lần login đầu|Quest đầu tiên phải gần hơn, rõ ràng hơn|
|Time to First Social Interaction|< 20 phút|Tăng mật độ người chơi ở starting zone, hoặc dùng NPC chat gợi ý|
|D1 Churn Reason (survey)|< 20% rời vì 'không hiểu phải làm gì'|Audit lại contextual help, thêm hướng dẫn tại điểm churn|


# **34. Retention Design — Thiết Kế Giữ Chân Người Chơi**
Retention là sinh mạng của game online. Acquisition đưa người chơi đến — Retention quyết định game có sống được không. Vietnam Online cần D30 retention > 25% để có cơ sở phát triển bền vững.
## **34.1. Retention Framework — Mô Hình 3 Vòng**

|**Vòng**|**Tên**|**Chu kỳ**|**Cơ chế chính**|**Mục tiêu**|
| :- | :- | :- | :- | :- |
|Vòng 1|Daily Hook|Hàng ngày (10-20 phút)|Daily quest, login reward, thời tiết thay đổi, NPC nhớ mình|D1-D7 retention|
|Vòng 2|Weekly Driver|Hàng tuần (1-3 giờ)|Weekly challenge, event cộng đồng, streak bonus, leaderboard reset|D7-D30 retention|
|Vòng 3|Long-term Arc|Hàng tháng / Mùa|Zone mới, seasonal event, progression milestones, B2B events|D30+ retention, LTV|
## **34.2. Daily Retention Mechanics**
### **34.2.1. Daily Login Reward — Hệ Thống Điểm Danh**

|**Ngày**|**Phần thưởng**|**Ghi chú**|
| :- | :- | :- |
|Ngày 1|100\.000 VND game|Cơ bản|
|Ngày 2|200\.000 VND game + 1 Stamina Boost||
|Ngày 3|300\.000 VND game||
|Ngày 4|Random cosmetic item nhỏ (filter, sticker)||
|Ngày 5|500\.000 VND game + 5 Xu Vàng|Milestone đáng kể|
|Ngày 6|300\.000 VND game + Experience Boost 2h||
|Ngày 7|1\.000.000 VND game + item giới hạn tuần + 10 Xu Vàng|Streak bonus quan trọng nhất|
|Ngày 30|Cosmetic hiếm + title 'Người Thường Trú' + 50 Xu Vàng|Milestone tháng — rất đáng để duy trì streak|

*Không phạt người chơi bỏ 1-2 ngày: streak chỉ reset về Ngày 1 nếu nghỉ > 3 ngày. Có 1 'Streak Shield' miễn phí mỗi tuần để bảo vệ streak khi bận.*
### **34.2.2. Daily Quests — Thiết Kế Chi Tiết**
- 3 quest mỗi ngày, reset lúc 0h, đa dạng không lặp lại trong tuần
- Thời gian hoàn thành: 10-20 phút mỗi quest, tổng daily routine < 45 phút
- Quest tích hợp với thế giới: 'Chụp ảnh hoàng hôn' chỉ làm được 17-19h thực tế — tạo lý do vào đúng giờ
- Quest theo vùng luân phiên: không chỉ ở zone đang chơi — gợi ý ghé zone khác nhau mỗi ngày
### **34.2.3. World State Changes — Thế Giới Thay Đổi Mỗi Ngày**
- Thời tiết khác nhau → ảnh đẹp khác nhau → lý do quay lại hôm nay thay vì hôm qua
- NPC có dialog mới mỗi ngày: tin tức địa phương giả, chuyện phiếm, sự kiện sắp tới
- Chợ có hàng quán mới: menu nhà hàng xoay vòng theo ngày, item đặc biệt chỉ bán thứ 6-7
- Photo feed cập nhật: ảnh cộng đồng mới nhất, tạo lý do để browse và tương tác
## **34.3. Weekly Retention Mechanics**

|**Mechanic**|**Mô tả**|**Lý do hiệu quả**|
| :- | :- | :- |
|Weekly Challenge|3 thử thách lớn/tuần: 'Đi xe máy 50km', 'Check-in 10 địa điểm mới', 'Tham gia 3 event cộng đồng'|Mục tiêu rõ ràng, thời hạn cụ thể, reward xứng đáng|
|Leaderboard Reset|Bảng xếp hạng Explorer Points, Photo Likes, Tour Revenue reset mỗi thứ Hai|Cơ hội đứng đầu cho người chơi mới, không bị dominated mãi|
|Featured Destination|Mỗi tuần 1 địa điểm được spotlight: bonus EXP 2x, NPC đặc biệt, quest riêng|Tạo điểm hội tụ, gặp gỡ cộng đồng|
|Weekend Events|Thứ 7-CN: chợ đêm đặc biệt, tour cộng đồng, mini-event 2 giờ|Phù hợp schedule người đi làm/học|
|Weekly Digest|Email/notification: 'Tuần này bạn đã làm được...' kèm ảnh đẹp nhất và bạn bè đang ở đâu|Reconnect người chơi không đăng nhập cuối tuần|
## **34.4. Long-term Retention — Vòng 3**
### **34.4.1. Progression Arcs (Cung Tiến Trình Dài Hạn)**

|**Arc**|**Milestone**|**Timeline ước tính**|
| :- | :- | :- |
|63 Tỉnh Thành|Check-in tại tất cả 63 tỉnh/thành Việt Nam|12-24 tháng chơi tích cực — cần game ra đủ zone|
|Master Photographer|Đạt Top 10 Photo Score, được featured trên front page|3-6 tháng tập trung vào photography|
|Legendary Guide|Dẫn 500 tour thành công với rating ≥ 4.8 sao|6-12 tháng đối với người chơi thường xuyên|
|Cultural Scholar|Hoàn thành 100% Cultural Knowledge cho 5 vùng|6 tháng, yêu cầu đọc thông tin địa điểm kỹ|
|Economy Baron|Đạt 1 tỷ VND game thông qua legitimate gameplay|Dài hạn, động lực cho người chơi nghiêm túc|
### **34.4.2. Seasonal Content Calendar**

|**Tháng**|**Sự kiện chính**|**Exclusive reward**|
| :- | :- | :- |
|Tháng 1-2|Tết Nguyên Đán — lớn nhất năm|Áo dài Tết, pet lợn/rồng/... theo năm, frame Tết|
|Tháng 3|Hội An Lantern Festival|Skin đèn lồng cho avatar, filter vàng ấm|
|Tháng 4|Giỗ Tổ Hùng Vương — quest lịch sử đặc biệt|Badge 'Con Rồng Cháu Lạc', title lịch sử|
|Tháng 5-6|Mùa du lịch hè — biển|Bikini/boardshorts cosmetic, skin thuyền kayak|
|Tháng 7|Lễ hội Kate (Ninh Thuận) — unique VN content|Trang phục Chăm Pa, item văn hóa độc đáo|
|Tháng 8-9|Trung Thu|Đèn lồng theo sau nhân vật, bánh Trung Thu item|
|Tháng 10-11|Hoa dã quỳ Đà Lạt — photography season|Camera filter đặc biệt, challenge ảnh hoa|
|Tháng 12|Giáng Sinh + Countdown 0h Tết Dương Lịch|Snowflake effects (hiếm ở VN), pháo hoa event|
## **34.5. Churn Prevention — Ngăn Người Chơi Bỏ Game**

|**Dấu hiệu churn**|**Phát hiện**|**Hành động**|
| :- | :- | :- |
|Không login 3 ngày|Analytics trigger|Push notification: 'NPC [tên] đang hỏi thăm bạn! Quay lại nhé'|
|Không login 7 ngày|Analytics trigger|Email cá nhân hóa: recap ảnh đẹp nhất, sự kiện đang diễn ra, bạn bè đang online|
|Login nhưng session < 5 phút liên tục 3 ngày|Engagement drop signal|Gợi ý tính năng chưa thử, quest mới phù hợp profile|
|Không có bạn bè sau ngày 7|Social isolation signal|Gợi ý join guild, giới thiệu 3 người chơi cùng sở thích|
|Hết tiền game và không có quest active|Friction signal|Auto-assign daily quest có reward cao, gợi ý cách kiếm tiền|
|Rating reputation giảm đột ngột|Quality signal|Gửi tip cải thiện dịch vụ, tạm thời giảm thấy của người này với service providers mới|


# **35. Infrastructure Architecture — Kiến Trúc Hạ Tầng**
Chương này là bản thiết kế kỹ thuật chi tiết của toàn bộ hạ tầng Vietnam Online — từ khi người chơi nhấn nút Play đến khi gói tin đầu tiên chạm đến server. Được thiết kế để bắt đầu với chi phí gần bằng 0 và scale lên hàng chục nghìn người chơi mà không cần redesign.
## **35.1. Tổng Quan Kiến Trúc (High-Level Architecture)**

|**Layer**|**Thành phần**|**Công nghệ**|**Ghi chú**|
| :- | :- | :- | :- |
|Client|Game client chạy trên máy người chơi|Unreal Engine 5 + C++|PC, sau đó Android, iOS|
|Edge / CDN|Asset delivery, DDoS protection, SSL termination|Cloudflare (free tier đủ dùng giai đoạn đầu)|Static asset cache tại edge gần người dùng VN|
|API Gateway|Một điểm vào duy nhất cho tất cả request|NGINX hoặc Cloudflare Tunnel|Rate limiting, auth check, route đến service đúng|
|Game Server|Xử lý real-time game state: vị trí, action, sync|UE5 Dedicated Server + C++ custom logic|1 server = 1 zone, auto-spawn khi zone đầy|
|Backend Services|REST API cho non-real-time: quest, inventory, economy|Go (Golang) microservices|Stateless, horizontal scale|
|AI Service|NPC AI Layer 3, recommendation engine|Python FastAPI + LLM API proxy|Separate từ game server để không ảnh hưởng latency|
|Database (Primary)|User data, characters, quests, economy, NPCs|PostgreSQL via Supabase|Managed DB, backup tự động, realtime subscriptions|
|Cache Layer|Session data, hot queries, leaderboards, rate limits|Redis (Upstash free tier hoặc self-hosted)|Sub-millisecond response cho data thường xuyên đọc|
|File Storage|UGC ảnh, game assets, backup files|Supabase Storage (S3-compatible)|Presigned URL cho upload trực tiếp từ client|
|Map Data|OSM tiles, Cesium terrain, POI database|Self-hosted tile server + Cesium Ion|Cache OSM tiles locally để giảm egress cost|
|Monitoring|Server health, error tracking, performance|Grafana + Prometheus + Sentry|Alert qua Telegram bot cho team nhỏ|
|CI/CD|Build, test, deploy tự động|GitHub Actions + Docker|Push to main = auto deploy staging|
## **35.2. Kiến Trúc Theo Giai Đoạn (Phased Architecture)**
### **35.2.1. Giai Đoạn 0-1 — Zero-Cost Prototype (0-50 CCU)**
*Mục tiêu: chứng minh concept hoạt động với ngân sách gần 0. Chấp nhận một số limitation.*

|**Thành phần**|**Giải pháp**|**Chi phí/tháng**|
| :- | :- | :- |
|Game Server|PC cá nhân hoặc laptop developer, port forward qua VPN (Tailscale)|0 đ|
|Database|Supabase Free Tier (500MB DB, 1GB file storage)|0 đ|
|Auth + Friends|Epic Online Services (EOS) Free Tier|0 đ|
|Map Data|Cesium Ion Free (up to 50GB streaming/tháng)|0 đ|
|CDN|Cloudflare Free|0 đ|
|Monitoring|Grafana Cloud Free (3 users, 14 ngày log)|0 đ|
|CI/CD|GitHub Actions (2000 phút/tháng free)|0 đ|
|TỔNG|—|0 – 500.000 đ/tháng (điện + internet)|
### **35.2.2. Giai Đoạn 2 — Small Scale (50-500 CCU)**

|**Thành phần**|**Giải pháp**|**Chi phí/tháng (ước tính)**|
| :- | :- | :- |
|Game Server|Oracle Cloud Free Tier: 4 OCPU ARM, 24GB RAM — đủ 2-3 zone song song|0 đ (mãi mãi với Oracle Always Free)|
|Database|Supabase Pro ($25/tháng): 8GB DB, no pause, daily backup|~600.000 đ|
|Cache|Upstash Redis Free (10.000 req/ngày) hoặc Redis trên Oracle VM|0 – 100.000 đ|
|CDN / DDoS|Cloudflare Pro ($20/tháng): WAF, image optimization|~500.000 đ|
|Monitoring|Grafana Cloud Free + self-hosted Prometheus trên Oracle|0 đ|
|AI NPC (Layer 3)|OpenAI API hoặc Anthropic API — chỉ cho ~100 NPC đặc biệt|~500.000 – 2.000.000 đ (tùy usage)|
|TỔNG|—|~2.000.000 – 4.000.000 đ/tháng|
### **35.2.3. Giai Đoạn 3+ — Scale Up (500-10.000+ CCU)**

|**Thành phần**|**Giải pháp**|**Ghi chú scale**|
| :- | :- | :- |
|Game Servers|Kubernetes cluster (GKE hoặc EKS), auto-scale game server pods|HPA dựa trên CCU/zone, cold start < 30 giây|
|Database|Supabase Pro + Read Replicas hoặc migrate sang self-managed RDS|Connection pooling (PgBouncer), read từ replica|
|Cache|Redis Cluster (Upstash Enterprise hoặc self-hosted)|Redis Cluster mode, 3 shards minimum|
|Load Balancer|Cloudflare Load Balancing hoặc AWS ALB|Sticky session cho game server connections|
|Map Tile Server|Self-hosted OpenMapTiles trên Hetzner EU (~20 EUR/tháng)|Cache tiles 1 tháng, Việt Nam toàn bộ ~50GB|
|AI Service|Dedicated Python service, queue-based với Redis|Rate limit per NPC, cost cap per ngày|
|CDN Assets|Cloudflare R2 + CDN: game assets, UGC photos|~0.015 USD/GB egress, rẻ hơn S3 nhiều|
## **35.3. Game Server Architecture — Chi Tiết**

|**Thành phần**|**Mô tả kỹ thuật**|**Thông số**|
| :- | :- | :- |
|Zone Instance|Mỗi zone (thành phố) có 1+ instance, tối đa 200 CCU/instance|RAM: ~2GB/instance, CPU: 1 core|
|Instance Manager|Service theo dõi CCU, auto-spawn instance mới khi > 80% capacity|Response time < 5 giây để spawn instance|
|World Server|Lưu global world state: weather, events, NPC schedules|Singleton, shared across all instances|
|Chat Server|WebSocket server xử lý proximity/zone/party chat|Separate từ game server để không block tick|
|Tick Rate|Server update 20 tick/giây cho movement, 5 tick/giây cho world state|Đủ mượt, tiết kiệm bandwidth|
|Interest Management|Mỗi người chơi chỉ nhận update từ người trong bán kính 200m|Giảm 80% network traffic so với broadcast toàn map|
|Dead Reckoning|Client tự dự đoán vị trí giữa các tick dựa trên velocity|Ẩn latency lên đến 200ms mà người chơi không nhận ra|
## **35.4. Network Protocol Design**

|**Loại data**|**Protocol**|**Lý do**|
| :- | :- | :- |
|Player position, rotation (real-time)|UDP với custom reliability layer (EOS Reliable UDP)|Ưu tiên tốc độ, mất 1 packet không sao|
|Chat messages|WebSocket (TCP)|Không được mất, thứ tự quan trọng|
|Quest, inventory, economy API|HTTPS REST (TCP)|Stateless, cacheable, dễ debug|
|NPC AI responses|HTTPS với streaming (Server-Sent Events)|Response dài, hiện từng từ như đang gõ|
|Live notifications|WebSocket persistent connection|Push từ server không cần client polling|
|Asset streaming (map tiles)|HTTP/2 với CDN cache|Multiplexing, nén tốt, CDN edge gần VN|
## **35.5. Database Schema — Kiến Trúc Dữ Liệu Tổng Thể**

|**Schema Group**|**Bảng chính**|**Estimated rows (1M users)**|
| :- | :- | :- |
|Auth & Users|users, sessions, devices, audit\_logs|1M / 10M / 3M / 50M|
|Characters|characters, appearances, positions, stats|1M / 1M / 1M / 1M|
|World|zones, locations, npc\_instances, world\_events|100 / 50K / 10K / 5K|
|Gameplay|quests, quest\_steps, inventories, achievements|50M / 200M / 20M / 100M|
|Economy|wallets, transactions, shop\_listings, tour\_bookings|1M / 500M / 100K / 10M|
|Social|friendships, parties, guilds, messages, reviews|50M / 5M / 100K / 1B / 50M|
|Content|poi\_data, ugc\_submissions, photos, tours|50K / 1M / 100M / 500K|
|Analytics|events (partitioned by day), session\_summaries|10B+ / 100M|

*Analytics table sử dụng TimescaleDB extension trên PostgreSQL hoặc tách riêng ra ClickHouse để xử lý time-series data hiệu quả hơn ở scale lớn.*
## **35.6. Disaster Recovery & Business Continuity**

|**Scenario**|**RTO (Recovery Time)**|**RPO (Data Loss)**|**Giải pháp**|
| :- | :- | :- | :- |
|Game server crash|< 30 giây|0 (stateless + checkpoint)|Auto-restart + player được teleport về safe zone|
|Database primary failure|< 5 phút|< 1 phút|Supabase tự động failover sang replica|
|Full datacenter outage|< 1 giờ|< 1 giờ|Snapshot hàng giờ, restore lên region backup|
|DDoS attack|< 15 phút mitigation|0|Cloudflare Under Attack Mode, IP block tự động|
|Bad deploy (critical bug)|< 10 phút rollback|0|Blue-green deployment, rollback 1 lệnh|
|Data corruption|< 4 giờ|< 1 giờ|Point-in-time recovery (PITR) trên Supabase Pro|
## **35.7. Cost Projection — Dự Trù Chi Phí Hạ Tầng**

|**Giai đoạn**|**CCU**|**Chi phí/tháng (USD)**|**Chi phí/tháng (VND ≈)**|
| :- | :- | :- | :- |
|Prototype (tháng 1-5)|0-50|$0 – $10|0 – 250.000 đ|
|Alpha (tháng 6-9)|50-200|$20 – $80|500\.000 – 2.000.000 đ|
|Beta (tháng 10-14)|200-1.000|$100 – $400|2\.500.000 – 10.000.000 đ|
|Soft Launch (tháng 15-20)|1\.000-5.000|$500 – $2.000|12\.500.000 – 50.000.000 đ|
|Scale (tháng 21-24)|5\.000-20.000|$2.000 – $8.000|50\.000.000 – 200.000.000 đ|
|Mature (năm 3+)|20\.000+|$8.000+|200\.000.000 đ+|

*Ở giai đoạn Mature, doanh thu B2B và Vietnam Pass cần trang trải chi phí hạ tầng. Break-even infrastructure ước tính khoảng 5.000 DAU với conversion rate Vietnam Pass 5% tại giá 50.000 đ/tháng.*



**— Hết Tài Liệu Bổ Sung Chương 31–35 —**

*Vietnam Online Project Documentation — v1.0*

PRD · Vision & Design Pillars · Onboarding · Retention · Infrastructure Architecture
Trang  / 
