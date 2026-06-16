VIETNAM ONLINE — Tài Liệu Thiết Kế — Chương 19–30

**VIETNAM ONLINE**

*Thế Giới Du Lịch Ảo Toàn Quốc Việt Nam*

Tài liệu thiết kế bổ sung — Chương 19–30 — v1.0


# **19. UI/UX Design — Thiết Kế Giao Diện & Trải Nghiệm Người Dùng**
Giao diện Vietnam Online phải phục vụ hai mục tiêu song song: không cản trở việc trải nghiệm thế giới 3D đẹp và đồng thời đủ rõ ràng để người chơi mới không bị lạc. Triết lý thiết kế: World First — UI Second.
## **19.1. Triết Lý Thiết Kế UI**

|**Nguyên tắc**|**Mô tả**|**Ví dụ áp dụng**|
| :- | :- | :- |
|Invisible UI|UI ẩn đi khi không cần — thế giới là trung tâm|HUD tự ẩn sau 10 giây không tương tác|
|Context-Aware|UI hiển thị đúng thứ người chơi cần lúc đó|Đến gần xe máy → hiện nút 'Thuê xe'|
|Mobile-First Thinking|Nút đủ lớn, tap target ≥ 44px dù là PC|Nút hành động chính đặt vùng ngón tay cái|
|Cultural Authenticity|Ngôn ngữ hình ảnh gợi Việt Nam, không generic|Minimap phong cách bản đồ giấy cổ|
|Progressive Disclosure|Hiện thông tin cơ bản trước, chi tiết khi cần|Tên địa điểm → tap → lịch sử đầy đủ|
|Zero Learning Curve|Chơi được ngay không cần đọc tutorial dài|Tutorial nhúng vào 5 phút đầu gameplay|
## **19.2. Cấu Trúc HUD (Heads-Up Display)**
HUD được chia thành 4 góc màn hình, mỗi góc một nhóm thông tin tương ứng:

|**Vị trí**|**Thông tin hiển thị**|**Điều kiện ẩn/hiện**|
| :- | :- | :- |
|Góc trái trên|Minimap (bản đồ giấy), La bàn, Vị trí GPS|Luôn hiển thị (có thể thu nhỏ)|
|Góc phải trên|Giờ trong game, Thời tiết, Tên khu vực đang ở|Luôn hiển thị|
|Góc trái dưới|Nhiệm vụ hiện tại (1-2 dòng), Tiến độ %|Ẩn khi không có quest active|
|Góc phải dưới|Wallet (VND game), Stamina bar, Ping server|Ẩn khi vào Photo Mode|
|Giữa dưới|Thanh hành động nhanh (xe, khách sạn, ăn uống, bản đồ)|Ẩn sau 8 giây không dùng|
|Giữa màn hình|Crosshair tương tác + tên vật thể có thể interact|Chỉ hiện khi nhìn vào object tương tác được|
## **19.3. Màn Hình & Flow Chính**

|**Màn hình**|**Mô tả**|**Transition**|
| :- | :- | :- |
|Main Menu|Video ambient Việt Nam làm nền động, menu nổi phía trước|Fade in từ loading|
|Character Creator|Xoay 3D real-time, slider trực quan, preview trang phục|Slide in từ phải|
|World Map|Bản đồ Việt Nam phong cách cổ, zoom in → enter zone|Camera bay xuống zone|
|Zone Loading|Ảnh concept art zone + tip văn hóa, không màn đen trắng|Crossfade vào world|
|Inventory / Backpack|Ba lô da mở ra, item 3D nổi trong không gian|Slide up từ dưới|
|Quest Journal|Sổ tay viết tay, bản đồ ghi chú, tab nhiệm vụ/thành tựu|Flip trang sách|
|Social / Friends|Avatar 3D bạn bè, trạng thái online, vị trí trong game|Slide in từ trái|
|Shop / Cosmetics|Mannequin 3D mặc trang phục, preview trên avatar mình|Modal overlay|
|Settings|Panel đơn giản, organized theo category, search settings|Slide in từ phải|
## **19.4. HUD Interaction Flow — Thuê Xe Máy**
Ví dụ luồng tương tác thuê xe máy — toàn bộ thực hiện không rời mắt khỏi thế giới 3D:

- Người chơi đi bộ đến trạm xe → HUD hiện icon xe máy nhấp nháy nhẹ ở góc màn hình
- Nhấn [E] hoặc tap icon → Panel nhỏ slide up từ dưới: danh sách xe, giá/giờ, nút Thuê
- Chọn xe → Confirm bằng 1 tap → Xe xuất hiện ngay bên cạnh nhân vật
- HUD cập nhật: hiện icon xe đang thuê + đồng hồ đếm giờ + nút Trả xe
- Toàn bộ flow: < 5 giây, không chuyển màn hình, không loading
## **19.5. Mobile UI Adaptations**

|**Thành phần**|**PC Layout**|**Mobile Layout**|
| :- | :- | :- |
|Di chuyển|WASD + chuột|Virtual joystick trái, camera swipe phải|
|Tương tác|Click / phím E|Tap vào object, hoặc nút tương tác phía phải|
|Minimap|128×128px góc trái trên|Tap để mở full-screen, thu nhỏ tự động|
|Chat|Bàn phím gõ, hiện bong bóng chat 3D|Phím tắt giọng nói, sticker preset|
|Inventory|Hover để xem tooltip|Long-press để xem detail|
|HUD size|Nhỏ gọn, không che view|Auto-scale 120% so với PC|
## **19.6. Accessibility trong UI**
(Chi tiết đầy đủ xem Chương 29 — Accessibility. Tóm tắt liên quan UI:)

- Tất cả text UI ≥ 14px, có tùy chọn tăng lên 200%
- Không dùng màu sắc là signal duy nhất — luôn kèm icon hoặc text
- Contrast ratio tối thiểu 4.5:1 cho text thường, 3:1 cho text lớn (WCAG AA)
- Hỗ trợ gamepad hoàn toàn: focus ring rõ, không cần chuột cho bất kỳ thao tác nào


# **20. Economy Design — Thiết Kế Hệ Thống Kinh Tế**
Vietnam Online có hệ thống kinh tế 2 lớp: VND Game (tiền trong game kiếm được qua gameplay) và Xu Vàng (tiền cao cấp mua bằng tiền thật). Mục tiêu: kinh tế cảm giác thật, cân bằng, không Pay-to-Win.
## **20.1. Hai Loại Tiền Tệ**

|**Loại**|**Tên**|**Kiếm bằng cách nào**|**Tiêu vào đâu**|
| :- | :- | :- | :- |
|Tiền game (soft)|VND (đơn vị: đồng)|Nhiệm vụ, nghề nghiệp, bán ảnh, bán tour|Thuê xe, ăn uống, khách sạn, vé tham quan, mua đặc sản|
|Tiền cao cấp (hard)|Xu Vàng (XV)|Mua bằng tiền thật hoặc thưởng sự kiện|Cosmetic, Vietnam Pass, slot lưu trữ thêm, emote đặc biệt|
|Điểm danh tiếng|Reputation (RP)|Được đánh giá tốt từ NPC và người chơi khác|Mở khóa nghề nghiệp cao cấp, địa điểm VIP, giảm giá dịch vụ|
## **20.2. Nguồn Thu Nhập Người Chơi (Income Sources)**

|**Nguồn thu**|**Số tiền ước tính (VND game/giờ)**|**Điều kiện**|
| :- | :- | :- |
|Hoàn thành nhiệm vụ chính|500\.000 – 2.000.000|Tùy độ khó nhiệm vụ|
|Hoàn thành nhiệm vụ phụ|50\.000 – 300.000|Mỗi nhiệm vụ|
|Hành nghề Hướng dẫn viên|200\.000 – 800.000|Tùy số người trong tour, đánh giá|
|Hành nghề Tài xế|50\.000 – 150.000|Mỗi chuyến đi|
|Bán ảnh trong game|10\.000 – 500.000|Tùy chất lượng ảnh, địa điểm hiếm|
|Chủ khách sạn / nhà hàng|Lên đến 5.000.000/ngày|Tùy lượng khách, đánh giá|
|Daily login bonus|100\.000 – 500.000|Streak liên tiếp nhận nhiều hơn|
|Khám phá địa điểm mới (lần đầu)|200\.000 – 1.000.000|Bonus đặc biệt cho người check-in đầu tiên|
## **20.3. Chi Tiêu Tiêu Biểu (Expense Simulation)**
Bảng giả lập chi phí một ngày du lịch trong game tại Hội An:

|**Hoạt động**|**Chi phí (VND game)**|**Ghi chú**|
| :- | :- | :- |
|Thuê xe máy (8 giờ)|80\.000|Loại xe phổ thông|
|Đặt phòng hostel (1 đêm)|150\.000|Phòng tập thể 6 người|
|Ăn sáng Cao Lầu|35\.000|Đặc sản Hội An|
|Vé tham quan Phố Cổ|120\.000|Gói 5 điểm|
|Cà phê + Bánh mì xíu mại|45\.000|Quán local|
|Quà lưu niệm đèn lồng|80\.000|Craft item|
|Bữa tối nhà hàng ven sông|200\.000|Set menu|
|Tổng chi phí 1 ngày|710\.000|Tương đương ~2-3 nhiệm vụ phụ|
## **20.4. Kinh Tế NPC & Player-Driven Economy**
- NPC có ngân sách riêng — NPC chủ quán trả tiền người chơi làm việc cho họ, không phải tiền từ hư không
- Player-to-Player: hướng dẫn viên bán tour, tài xế bán chuyến, nhiếp ảnh gia bán ảnh trực tiếp cho người chơi khác
- Thị trường ảnh (Photo Market): người chơi upload ảnh đẹp → người khác mua → người chụp nhận 80% tiền, 20% vào quỹ game
- Giá cả điều chỉnh theo cung cầu: địa điểm đông khách → giá thuê xe, khách sạn tăng nhẹ (dynamic pricing mô phỏng thực tế)
## **20.5. Anti-Inflation Controls**

|**Vấn đề**|**Cơ chế kiểm soát**|
| :- | :- |
|Tiền bơm vào quá nhiều|Thuế giao dịch 5% trên mọi player-to-player transaction|
|Người chơi trữ tiền, không tiêu|Phí bảo quản khách sạn, bãi giữ xe theo ngày|
|Bot farm kiếm tiền|Rate limit quest hoàn thành, anti-bot detection (xem Chương 27)|
|Inflation item hiếm|Item limited edition không re-issue, supply cố định|
|Dump lượng lớn tiền vào thị trường|Giới hạn giao dịch tối đa 10.000.000 VND game/ngày|


# **21. Quest & Progression Design — Thiết Kế Nhiệm Vụ & Tiến Trình**
Hệ thống quest của Vietnam Online không giống game RPG truyền thống. Không có quest 'giết 10 con quái'. Mọi nhiệm vụ đều gắn với du lịch, khám phá, gặp gỡ người thật và trải nghiệm văn hóa.
## **21.1. Phân Loại Nhiệm Vụ**

|**Loại**|**Mô tả**|**Ví dụ**|**Phần thưởng chính**|
| :- | :- | :- | :- |
|Main Quest|Hành trình khám phá từng tỉnh/thành, unlock zone mới|"Hội An — Thành Phố Ký Ức": tham quan 10 điểm, gặp 5 NPC đặc biệt|Mở zone mới, title đặc biệt, Xu Vàng|
|Side Quest|Câu chuyện nhỏ từ NPC địa phương|"Bà Năm mất mèo": tìm mèo trong chợ, nhận phần thưởng đặc sản|VND game, item địa phương|
|Daily Quest|Nhiệm vụ hàng ngày reset lúc 0h|"Thử 3 quán cà phê hôm nay", "Chụp 5 ảnh trước 7h sáng"|VND game, EXP, streak bonus|
|Weekly Challenge|Thử thách tuần yêu cầu nhiều nỗ lực|"Đi xe máy 100km trong 1 tuần", "Gặp 20 người chơi khác nhau"|Item hiếm, Xu Vàng, cosmetic|
|Community Quest|Toàn server cùng hoàn thành một mục tiêu|"Cộng đồng check-in 1.000 lượt Tháp Bà Ponagar tuần này"|Buff toàn server, sự kiện mở khóa|
|Hidden Quest|Khám phá bằng tương tác bất ngờ|Nói chuyện đúng thứ tự với 3 NPC bí ẩn → quest bí mật mở ra|Achievement hiếm, title ẩn|
|Seasonal Quest|Chỉ có trong dịp lễ hội thật|Tết: "Thu thập 5 bao lì xì từ NPC trong 3 ngày"|Cosmetic Tết giới hạn, pet đặc biệt|
## **21.2. Hệ Thống Progression (Tiến Trình Nhân Vật)**
Vietnam Online không có level combat. Thay vào đó là hệ thống chuyên môn đa chiều:

|**Chỉ số tiến trình**|**Cách tăng**|**Lợi ích khi tăng cao**|
| :- | :- | :- |
|Explorer Level (1-100)|Check-in địa điểm mới, hoàn thành quest khám phá|Mở khóa khu vực ít người biết, title "Phượt Thủ Huyền Thoại"|
|Social Reputation (0-5 sao)|Đánh giá từ người chơi và NPC sau khi cung cấp dịch vụ|Giảm giá dịch vụ NPC, unlock vị trí đặc biệt|
|Profession Skill (1-10)|Thực hành nghề nghiệp liên tục|Dịch vụ chất lượng cao hơn, giá bán cao hơn, khách nhiều hơn|
|Cultural Knowledge (0-1000)|Đọc thông tin lịch sử, hoàn thành quiz văn hóa, nói chuyện NPC|NPC chia sẻ thông tin ẩn, unlock câu chuyện đặc biệt|
|Photo Score (ELO)|Ảnh được cộng đồng like và mua|Rank nhiếp ảnh gia, featured trên feed chính|
|Traveler Stamp (0-63)|Đến đủ 63 tỉnh/thành|Title "Người Việt Toàn Quốc", frame avatar đặc biệt|
## **21.3. Quest Design Framework**
Mỗi quest được thiết kế theo công thức 4C:

- Context: Tại sao quest này tồn tại? (Gắn với câu chuyện NPC hoặc lịch sử địa điểm thật)
- Challenge: Người chơi phải làm gì? (Khám phá, di chuyển, tương tác, chụp ảnh, giao tiếp)
- Choice: Có ít nhất 2 cách giải quyết không? (Đi bộ vs xe máy, hỏi NPC A vs NPC B)
- Consequence: Lựa chọn ảnh hưởng đến outcome như thế nào? (Đường nào nhanh hơn? NPC nào cho thêm thông tin?)
## **21.4. Quest Tracker & Navigation**
- Quest log: sổ tay viết tay, tab Active / Completed / Hidden, search và filter theo zone
- Navigation: waypoint trên minimap, không hiện mũi tên thẳng — hiện gợi ý 'Đi về hướng Đông, hỏi người dân'
- Difficulty indicator: ký hiệu ba nón lá (1 nón = dễ, 3 nón = thử thách)
- No quest spam: tối đa 3 quest active cùng lúc để người chơi tập trung


# **22. Social Systems — Hệ Thống Xã Hội**
Vietnam Online là game về con người gặp con người. Hệ thống xã hội là linh hồn của dự án — không có nó, thế giới chỉ là một bảo tàng 3D đẹp nhưng trống rỗng.
## **22.1. Các Kênh Giao Tiếp**

|**Kênh**|**Phạm vi**|**Công nghệ**|**Giai đoạn**|
| :- | :- | :- | :- |
|Proximity Chat (text)|Bán kính 50m trong game|Server relay, filter nội dung|Giai đoạn 1|
|Party Chat (text)|Nhóm tối đa 8 người|Dedicated channel per party|Giai đoạn 1|
|Zone Chat|Tất cả người trong cùng zone/thành phố|Broadcast với rate limit|Giai đoạn 2|
|Whisper (DM)|1-1, bất kỳ đâu trên server|Direct message|Giai đoạn 1|
|Proximity Voice|Nghe nhỏ dần khi ở xa, to dần khi đến gần|Vivox / Agora spatial audio|Giai đoạn 3|
|Party Voice|Rõ ràng bất kể khoảng cách trong party|Vivox group channel|Giai đoạn 3|
|Emote / Gesture|Vẫy tay, chào, cười — nhìn thấy quanh bán kính 30m|Animation sync qua server|Giai đoạn 1|
## **22.2. Hệ Thống Bạn Bè & Nhóm**
- Friend list tối đa 200 người, thấy vị trí bạn trên bản đồ (nếu bạn cho phép)
- Party (nhóm du lịch): 2-8 người, cùng spawn, chia sẻ quest progress, nhìn thấy nhau trên minimap riêng
- Guild / Hội Du Lịch: tối đa 50 thành viên, có chat riêng, leaderboard khám phá nội bộ, huy hiệu guild
- Block / Report: tức thì ẩn người chơi trong thế giới 3D, báo cáo nội dung vi phạm
## **22.3. Hệ Thống Chụp Ảnh Cộng Đồng**

|**Tính năng**|**Mô tả**|
| :- | :- |
|Chụp ảnh chung (Group Photo)|Cả nhóm đứng vào khung, chọn bố cục, NPC hỗ trợ chụp nếu cần|
|Photo Feed|Dòng ảnh cộng đồng trong game, like / comment / save|
|Photo Contest|Cuộc thi ảnh theo chủ đề hàng tuần, cộng đồng vote, người thắng nhận Xu Vàng|
|Photo Location Tag|Ảnh tự động tag địa điểm và giờ chụp, xem được trên bản đồ|
|Share ngoài game|Export ảnh có watermark Vietnam Online, chia sẻ lên MXH thật|
## **22.4. Hệ Thống Sự Kiện Cộng Đồng**
- Player-organized events: người chơi tạo sự kiện gặp mặt, tour nhóm, cuộc thi ảnh — cần 5+ người đăng ký
- Dev-organized events: lễ hội mùa vụ (Tết, Trung Thu), sự kiện mở zone mới, world boss... không, ý mình là world event như thiên tai ảo hưởng cả server
- Event calendar tích hợp: hiển thị sự kiện trong 7 ngày tới ngay trên màn hình chính
- Flash mob: thông báo bí ẩn gợi ý địa điểm → người chơi tập trung → xảy ra sự kiện ngắn
## **22.5. Hệ Thống Đánh Giá & Reputation**
- Sau mỗi chuyến taxi, tour, thuê nhà: cả hai bên đánh giá nhau 1-5 sao + bình luận
- Reputation public: người khác thấy điểm trung bình khi xem profile
- Hệ quả điểm thấp: NPC từ chối hợp tác, giá dịch vụ đắt hơn, bị gắn cờ review
- Hệ quả điểm cao: ưu tiên hiển thị dịch vụ, badge uy tín, unlock nghề nghiệp cao cấp


# **23. Transportation Simulation — Mô Phỏng Giao Thông**
Hệ thống giao thông là một trong những điểm phân biệt Vietnam Online với mọi game khác. Không chỉ là phương tiện di chuyển — mà là mô phỏng văn hóa giao thông Việt Nam.
## **23.1. Kiến Trúc Hệ Thống Giao Thông**

|**Layer**|**Thành phần**|**Mô tả kỹ thuật**|
| :- | :- | :- |
|Layer 1 — Network|Road graph từ OSM|Đường phố thật từ OpenStreetMap, phân loại: đường quốc lộ, tỉnh lộ, phố nhỏ, ngõ|
|Layer 2 — Traffic|NPC vehicles + Rules|NPC xe máy, ô tô, xe buýt di chuyển theo luật giao thông, tắc đường giờ cao điểm|
|Layer 3 — Physics|Vehicle handling|Mỗi loại xe có physics khác nhau: xe đạp nhẹ, xe máy nhanh, ô tô rộng không vào hẻm nhỏ|
|Layer 4 — Service|Booking & Rental|API thuê xe, đặt taxi, mua vé tàu/máy bay — kết nối với Economy system|
|Layer 5 — Simulation|Dynamic traffic|Tắc đường thật vào 7-9h và 17-19h, mưa làm giao thông chậm 30%|
## **23.2. Chi Tiết Từng Phương Tiện**

|**Phương tiện**|**Physics đặc trưng**|**Cơ chế đặc biệt**|**Độ ưu tiên triển khai**|
| :- | :- | :- | :- |
|Đi bộ|Character controller chuẩn, có mệt mỏi nếu chạy lâu|Stamina bar, hỏi đường NPC|MVP|
|Xe đạp|Nhẹ, nhanh trên đường bằng, chậm lên dốc|Thuê tại trạm, trả bất kỳ trạm nào trong zone|MVP|
|Xe máy|Linh hoạt nhất, vào hẻm nhỏ, léo lách giao thông|Đổ xăng, chọn loại (số/tay ga), tùy chỉnh màu|Giai đoạn 1|
|Xích lô|Chậm, chỉ trong phố cổ, góc nhìn cao đặc biệt|NPC xích lô dẫn tour tự động theo tuyến cố định|Giai đoạn 2|
|Taxi / Grab|Ô tô, không cần lái, chọn điểm đến|Đặt qua app trong game, chia sẻ chuyến carpooling|Giai đoạn 2|
|Xe buýt|Tuyến cố định, dừng đúng trạm, lịch giờ thật|Xem lịch, mua vé, chờ tại trạm, push notify khi xe đến|Giai đoạn 2|
|Tàu hỏa|Liên tỉnh, lịch cố định, không thể tắt ngang|Đặt vé, chọn toa/khoang, xem phong cảnh từ cửa sổ|Giai đoạn 3|
|Máy bay|Tức thì (sau cutscene cất cánh/hạ cánh)|Check-in, cổng lên máy bay, cabin class khác nhau|Giai đoạn 3|
|Tàu/Cano|Physics nước, sóng ảnh hưởng|Du lịch biển đảo Phú Quốc, chèo thuyền Hạ Long|Giai đoạn 3|
## **23.3. Mô Phỏng Giao Thông Việt Nam**
Điểm đặc sắc nhất của hệ thống — giao thông Việt Nam có những đặc điểm riêng cần mô phỏng:

- Hẻm nhỏ: mạng lưới hẻm sâu trong các thành phố, chỉ xe máy và người đi bộ vào được
- Vỉa hè đa năng: xe máy đậu trên vỉa hè, quán cà phê chiếm vỉa hè, người đi bộ phải xuống lòng đường
- Giao thông hỗn hợp: xe máy, xích lô, ô tô, xe đạp, người đi bộ cùng đường — NPC AI xử lý tình huống
- Giờ cao điểm: 7-9h và 17-19h traffic tăng 300%, tốc độ di chuyển giảm, NPC taxi tăng giá surge
- Thời tiết ảnh hưởng: mưa → đường trơn → xe máy đi chậm hơn, một số đường ngập nước tạm thời bị chặn
## **23.4. Navigation System (GPS Trong Game)**
- Bản đồ trong game dùng dữ liệu OSM thật — đường tên thật, hẻm thật
- GPS navigation: chọn điểm đến → hiện tuyến đường trên minimap, không có mũi tên khổng lồ cản view
- Chế độ khám phá: tắt GPS, chỉ dùng hỏi đường NPC — thưởng EXP explorer cao hơn 50%
- Offline map: có thể 'mua bản đồ giấy' trong game để xem ngay cả khi mất tín hiệu GPS game


# **24. Tourism System — Hệ Thống Du Lịch**
Đây là hệ thống cốt lõi phân biệt Vietnam Online với mọi game MMO khác. Mỗi địa điểm du lịch không chỉ là một điểm check-in — mà là một trải nghiệm đầy đủ có chiều sâu văn hóa và lịch sử.
## **24.1. Cấu Trúc Dữ Liệu Địa Điểm (POI)**

|**Trường dữ liệu**|**Mô tả**|**Nguồn**|
| :- | :- | :- |
|Tên chính thức|Tên đầy đủ chính xác bằng tiếng Việt và Anh|Wikipedia / Cục Du Lịch|
|Tọa độ GPS|Lat/Lng chính xác đến 6 chữ số thập phân|OpenStreetMap|
|Loại địa điểm|Lịch sử / Tự nhiên / Tâm linh / Ẩm thực / Giải trí / Di sản|Manual tag|
|Giờ hoạt động|Theo ngày trong tuần, lễ tết, mùa vụ|Manual + OSM|
|Giá vé|Người lớn / Trẻ em / Người cao tuổi / Người nước ngoài|Manual|
|Thông tin lịch sử|500-2000 từ, đa ngôn ngữ, có ảnh minh họa|Biên tập thủ công|
|Ảnh tham chiếu|10-30 ảnh từ nhiều góc độ, theo mùa|UGC + Official|
|Điều kiện thời tiết lý tưởng|Mùa nào đẹp nhất, tránh mùa nào|Manual|
|Rating cộng đồng|Điểm 1-5 sao, phân tích theo tiêu chí|Player-generated|
|Quest liên kết|Danh sách quest gắn với địa điểm này|Quest system link|
## **24.2. Hệ Thống Check-In & Khám Phá**

|**Tính năng**|**Mô tả chi tiết**|
| :- | :- |
|First Discovery Bonus|Người đầu tiên check-in một địa điểm nhận 5x EXP và badge "Người Khám Phá" vĩnh viễn|
|Check-in Requirements|Phải ở trong bán kính 50m của địa điểm, có thể yêu cầu thêm điều kiện (giờ đúng, thời tiết phù hợp)|
|Photo Check-in|Chụp ảnh tại địa điểm → tự động xác nhận check-in + lưu vào hành trình|
|Hành Trình (Journey Log)|Timeline tự động ghi lại mọi nơi đã đến, khoảng cách đi được, ảnh đã chụp|
|Passport Stamp|Mỗi tỉnh/thành có con dấu riêng, thu thập đủ 63 con dấu = thành tựu "Người Việt Toàn Quốc"|
|Hidden Locations|~20% địa điểm không hiện trên bản đồ — chỉ mở ra khi nghe NPC mách, giải mã clue ẩn|
## **24.3. Hướng Dẫn Viên & Tour System**
- Player guide: người chơi có Profession Skill hướng dẫn viên ≥ 5 có thể tạo và bán tour
- Tour builder: chọn điểm đến → thêm waypoint → viết mô tả → đặt giá → publish
- Booking: người chơi khác đặt tour, lên lịch, tập hợp điểm xuất phát
- Khi tour đang diễn ra: hướng dẫn viên dẫn đầu, cả nhóm nhìn thấy nhau, có icon đặc biệt
- Rating sau tour: đánh giá 2 chiều, hướng dẫn viên giỏi leo bảng xếp hạng toàn server
- NPC tour guide: cho người chơi muốn khám phá một mình, chi phí bằng VND game, script được viết tay
## **24.4. Hệ Thống Lưu Trú Chi Tiết**

|**Loại hình**|**Giá/đêm (VND game)**|**Đặc điểm gameplay**|
| :- | :- | :- |
|Hostel / Nhà nghỉ|100\.000 – 200.000|Phòng tập thể, gặp người chơi khác, có NPC staff thân thiện|
|Khách sạn 2-3 sao|300\.000 – 600.000|Phòng riêng, dịch vụ cơ bản, có NPC lễ tân|
|Boutique Hotel|700\.000 – 1.500.000|Thiết kế địa phương độc đáo, bonus EXP văn hóa khi ở|
|Resort 5 sao|2\.000.000 – 5.000.000|Hồ bơi, spa, nhà hàng trong resort, view cực đẹp|
|Homestay|150\.000 – 400.000|Ở với NPC gia đình, học món ăn địa phương, trải nghiệm bản địa sâu nhất|
|Camping|50\.000 – 100.000|Chỉ ở điểm thiên nhiên, dựng lều, lửa trại, bonus EXP explorer|


# **25. Dynamic World System — Hệ Thống Thế Giới Động**
Thế giới Vietnam Online không đứng yên chờ người chơi. Nó sống, thay đổi theo giờ, ngày, mùa và sự kiện thực tế. Đây là yếu tố khiến người chơi quay lại mỗi ngày để thấy điều mới.
## **25.1. Hệ Thống Thời Gian**

|**Tham số**|**Giá trị**|**Mô tả**|
| :- | :- | :- |
|Tỷ lệ thời gian|1:1 với thực tế|Giờ trong game = giờ thực ngoài đời, đồng bộ theo múi giờ GMT+7|
|Chu kỳ ngày/đêm|24 giờ thực|Bình minh, buổi sáng, trưa, chiều, hoàng hôn, đêm sớm, khuya|
|Ngày trong tuần|Thứ 2 đến Chủ Nhật|Thứ 7/CN: chợ đặc biệt, đông người hơn, sự kiện cuối tuần|
|Tháng & Mùa|12 tháng thực|Mùa mưa/khô, thay đổi vegetation, điều kiện du lịch|
|Lịch âm|Đồng bộ lịch âm thật|Rằm: chùa đông hơn, NPC cúng lễ; Mồng 1: lễ hội đặc biệt|
## **25.2. Hệ Thống Thời Tiết Động**

|**Loại thời tiết**|**Tác động gameplay**|**Tần suất (tùy vùng/mùa)**|
| :- | :- | :- |
|Nắng trong|Không tác động, ảnh chụp đẹp nhất|Thường xuyên (miền Nam mùa khô)|
|Nắng gắt (trưa hè)|Stamina giảm 20% khi đi bộ ngoài trời, NPC nghỉ trưa|Trưa hè miền Trung, Nam|
|Mưa nhẹ|Một số địa điểm ngoài trời đóng cửa, taxi đắt hơn 20%|Chiều miền Nam, Tây Nguyên|
|Mưa lớn / Bão|Máy bay hoãn, tàu biển ngừng, đường ngập, sự kiện đặc biệt|Mùa bão miền Trung|
|Sương mù|Tầm nhìn giảm, ảnh đẹp kiểu lãng mạn, speed giảm|Sáng sớm Đà Lạt, Sapa|
|Nhiều mây|Không ảnh hưởng lớn, ánh sáng diffuse đẹp cho ảnh|Mùa đông miền Bắc|
## **25.3. Hệ Thống Lễ Hội & Sự Kiện Theo Lịch**

|**Sự kiện**|**Thời gian (thực tế)**|**Thay đổi trong game**|
| :- | :- | :- |
|Tết Nguyên Đán|Tháng 1-2 âm lịch (5-7 ngày)|Phố xá trang trí đỏ vàng, pháo hoa 0h, NPC chúc Tết, lì xì quest, cosmetic giới hạn|
|Tết Trung Thu|Rằm tháng 8 âm lịch (1-3 ngày)|Đèn lồng bay, trẻ em NPC rước đèn, bán bánh trung thu, múa lân|
|Festival Huế|Tháng 4-5 (năm chẵn)|Biểu diễn nghệ thuật cung đình tại Huế, quest đặc biệt, NPC hoàng gia|
|Lễ hội Kate (Ninh Thuận)|Tháng 7 âm lịch|Trang phục Chăm Pa, lễ rước, âm nhạc đặc biệt|
|Tết Dương Lịch|1/1 hàng năm|Countdown, pháo hoa, event toàn server|
|Ngày Quốc Khánh|2/9|Diễu hành NPC, sự kiện quest lịch sử đặc biệt|
|Mùa hoa dã quỳ Đà Lạt|Tháng 11-12|Hoa nở rực khắp Đà Lạt, ảnh đẹp, tour hoa nở đặc biệt|
## **25.4. World Events (Sự Kiện Thế Giới Ngẫu Nhiên)**
- Lễ hội đường phố bất ngờ: NPC tổ chức biểu diễn nghệ thuật ngẫu nhiên tại quảng trường 30-60 phút
- Phiên chợ đêm đặc biệt: xuất hiện không thường xuyên, bán item hiếm không có ở chợ thường
- Thiên tai nhỏ (simulation): ngập đường cục bộ do mưa lớn → tuyến giao thông tạm thời thay đổi
- NPC vắng mặt: một số NPC có lịch sống — đôi khi 'đi công tác' vài ngày, không có tại vị trí thường


# **26. Content Pipeline — Quy Trình Sản Xuất Nội Dung**
Vietnam Online cần lượng content khổng lồ: hàng ngàn địa điểm, NPC, quest, dialog. Pipeline chuẩn hóa là cách duy nhất để duy trì chất lượng khi scale.
## **26.1. Tổng Quan Pipeline**

|**Giai đoạn**|**Input**|**Công việc**|**Output**|
| :- | :- | :- | :- |
|1\. Data Collection|OSM, Wikipedia, Cục Du Lịch, ảnh thực địa|Scrape, verify, format, tag|Raw POI database|
|2\. Content Authoring|Raw POI data + brief từ lead designer|Biên tập text, viết dialog NPC, thiết kế quest|Content sheet (JSON/Markdown)|
|3\. Art Production|Content sheet + style guide Chương 18|Model 3D, texture, VFX, UI asset|Game-ready assets (.uasset)|
|4\. Integration|Assets + Content sheet|Import vào UE5, link với system (quest, NPC, economy)|Playable build|
|5\. QA|Playable build|Functional test, visual review, localization check, performance|Bug list + sign-off|
|6\. Deployment|Signed-off build|Deploy lên server, update content database|Live content|
|7\. UGC Review|Player submissions|Moderation, verify, approve/reject|Community content live|
## **26.2. Tiêu Chuẩn Nội Dung Địa Điểm (POI)**

|**Tier**|**Số địa điểm mục tiêu**|**Mức độ chi tiết**|
| :- | :- | :- |
|Tier 1 — Flagship|50 địa điểm (di sản quốc gia, biểu tượng)|Model 3D tùy chỉnh hoàn toàn, interior có thể vào được, 2000+ từ lịch sử, voice-over tiếng Việt|
|Tier 2 — Standard|500 địa điểm (điểm du lịch phổ biến)|Model 3D semi-custom, exterior chi tiết, 500-1000 từ mô tả, text info đầy đủ|
|Tier 3 — Basic|5000+ địa điểm (quán ăn, chùa nhỏ, công viên)|Asset thư viện + custom texture, marker trên bản đồ, thông tin cơ bản từ OSM|
|Tier 4 — Procedural|Toàn bộ địa điểm còn lại|Auto-generated từ OSM data, không nội dung tùy chỉnh, có thể được nâng tier bởi community|
## **26.3. User-Generated Content (UGC) Pipeline**
- Người chơi submit: ảnh thực địa, thông tin địa điểm, review, tour custom, NPC dialog đề xuất
- Moderation queue: AI pre-filter nội dung vi phạm → human review → approve/reject trong 48h
- Reward system: UGC được approve → người submit nhận EXP và badge 'Đóng Góp Cộng Đồng'
- Upgrade tier: đủ UGC chất lượng tốt → POI Tier 4 được tự động upgrade lên Tier 3
- UGC moderation team: 2-3 người part-time ban đầu, scale theo lượng submission
## **26.4. Localization Pipeline**

|**Ngôn ngữ**|**Ưu tiên**|**Ghi chú**|
| :- | :- | :- |
|Tiếng Việt|P0 — Core|Tất cả content phải có TV trước, proofreading bắt buộc|
|Tiếng Anh|P1 — Launch|Dịch song song với TV từ giai đoạn 1|
|Tiếng Nhật|P2 — Post-launch|Thị trường khách du lịch Nhật lớn tại Việt Nam|
|Tiếng Hàn|P2 — Post-launch|Cộng đồng người Hàn tại VN và du khách Hàn Quốc|
|Tiếng Trung (phồn thể)|P3 — Expansion|Thị trường Đài Loan, Hồng Kông|
|Tiếng Pháp|P4 — Future|Di sản Pháp tại VN, thị trường châu Âu Pháp ngữ|


# **27. Security & Anti-Cheat — Bảo Mật & Chống Gian Lận**
Vietnam Online không phải game combat, nhưng hệ thống kinh tế player-driven và reputation system tạo động lực mạnh để gian lận. Bảo mật phải được thiết kế từ đầu, không phải vá sau.
## **27.1. Mô Hình Bảo Mật Tổng Thể**

|**Layer**|**Thành phần**|**Mục tiêu**|
| :- | :- | :- |
|Network|TLS 1.3 cho mọi kết nối, Certificate pinning trên client|Chặn man-in-the-middle, packet sniffing|
|Authentication|JWT token ngắn hạn (15 phút), Refresh token (7 ngày), 2FA optional|Chống chiếm tài khoản|
|Authorization|RBAC (Role-Based Access Control) cho mọi API endpoint|Player/GM/Admin có quyền khác nhau|
|Game Logic|Server-authoritative: mọi action game quan trọng verify trên server|Chặn client-side hack|
|Data|AES-256 data at rest, bcrypt password, không log sensitive data|Bảo vệ dữ liệu người chơi|
|Infrastructure|WAF, Rate limiting, DDoS protection (Cloudflare)|Bảo vệ uptime server|
|Monitoring|Anomaly detection real-time, alert khi phát hiện bất thường|Phát hiện sớm, phản ứng nhanh|
## **27.2. Anti-Cheat Cụ Thể cho Vietnam Online**

|**Loại gian lận**|**Phát hiện**|**Xử lý**|
| :- | :- | :- |
|Speed hack (di chuyển quá nhanh)|Server kiểm tra tốc độ di chuyển, alert nếu > 150% max speed 3 lần/phút|Cảnh báo → teleport về vị trí hợp lệ → ban 24h → ban vĩnh viễn|
|Teleport hack|Vị trí thay đổi đột ngột > 500m/giây không hợp lệ (không phải taxi/máy bay)|Rollback vị trí, ghi log, điều tra|
|Economy exploit (kiếm tiền bất thường)|Monitor thu nhập/giờ, so sánh với baseline theo nghề nghiệp|Flag account, freeze wallet, manual review|
|Bot farming quest|Pattern detection: hành động lặp lại quá đều, không có mouse jitter, thời gian phản ứng < 100ms|CAPTCHA thách thức, nếu fail → suspend|
|Fake review manipulation|Multiple accounts từ cùng IP/device đánh giá nhau|Vô hiệu hóa reviews từ IP cluster đó, penalize tài khoản|
|Duplicate account|Device fingerprint + IP + behavior pattern matching|Link các tài khoản, xử lý nếu vi phạm ToS|
|Exploit địa điểm (out of bounds)|Physics validation trên server, không thể ở vị trí không hợp lệ > 3 giây|Teleport về vị trí an toàn gần nhất|
## **27.3. Bảo Mật Tài Khoản Người Chơi**
- Đăng nhập: email/password + optional Google/Facebook OAuth
- 2FA: hỗ trợ TOTP (Google Authenticator), SMS (Việt Nam), Email OTP
- Session management: đăng xuất từ xa tất cả thiết bị, xem lịch sử đăng nhập
- Phát hiện đăng nhập bất thường: IP mới, quốc gia mới → gửi email xác nhận
- Khôi phục tài khoản: qua email xác thực hoặc hỗ trợ thủ công sau 72h
## **27.4. GM Tools & Moderation**

|**Tool**|**Chức năng**|**Quyền truy cập**|
| :- | :- | :- |
|Player Lookup|Xem toàn bộ lịch sử, vị trí, giao dịch của 1 player|GM+|
|Freeze Account|Tạm khóa tài khoản trong khi điều tra|GM+|
|Rollback Transaction|Hoàn trả giao dịch economy sai trong 24h|Admin|
|Teleport Player|Di chuyển player đến vị trí an toàn khi bị stuck|GM+|
|Broadcast Message|Thông báo toàn server hoặc 1 zone|GM+|
|Ban Management|Ban theo thời gian hoặc vĩnh viễn, ghi lý do|GM+|
|Audit Log|Xem tất cả hành động của GM và Admin|Admin only|


# **28. Analytics & Telemetry — Phân Tích Dữ Liệu & Đo Lường**
Không thể cải thiện điều không đo được. Analytics cho phép nhóm phát triển hiểu người chơi đang làm gì, tại sao họ nghỉ game, và điều gì khiến họ quay lại.
## **28.1. Stack Analytics**

|**Công cụ**|**Mục đích**|**Chi phí**|
| :- | :- | :- |
|PostHog (self-hosted)|Event tracking, funnel analysis, session recording, feature flags|Miễn phí (self-hosted), ~$50/tháng server|
|Grafana + Prometheus|Server metrics: CPU, RAM, latency, CCU, error rate theo thời gian thực|Miễn phí (open source)|
|Supabase Analytics|Database query performance, slow query detection|Có sẵn trong Supabase|
|Custom Dashboard|Business metrics: DAU, MAU, ARPU, Retention D1/D7/D30|Tự xây bằng Grafana|
|Sentry|Error tracking phía client và server, stack trace, user impact|Miễn phí tier đủ dùng giai đoạn đầu|
## **28.2. Events Cần Track (Game Telemetry)**

|**Category**|**Events quan trọng**|**Tại sao cần**|
| :- | :- | :- |
|Onboarding|tutorial\_start, tutorial\_step\_N, tutorial\_complete, tutorial\_skip|Tìm điểm người mới bỏ cuộc trong tutorial|
|Exploration|poi\_visited, zone\_entered, first\_discovery, hidden\_found|Biết vùng nào ít được khám phá → cần thêm content|
|Economy|item\_purchased, tour\_sold, photo\_sold, wallet\_balance\_daily|Phát hiện inflation, exploit, content nào tạo ra giá trị|
|Social|friend\_added, party\_formed, message\_sent, event\_joined|Đo sức khỏe cộng đồng, feature social nào được dùng|
|Retention|session\_start, session\_end, session\_duration, days\_since\_last\_login|D1/D7/D30 retention, dự đoán churn|
|Performance|fps\_average, latency\_ms, crash\_event, load\_time\_zone|Phát hiện vấn đề kỹ thuật ảnh hưởng trải nghiệm|
|Monetization|purchase\_initiated, purchase\_complete, purchase\_failed, vietnam\_pass\_sub|Tối ưu conversion funnel, tìm điểm friction|
## **28.3. KPIs Chính theo Giai Đoạn**

|**KPI**|**Mục tiêu Giai đoạn 1**|**Mục tiêu Giai đoạn 3**|**Công thức**|
| :- | :- | :- | :- |
|DAU (Daily Active Users)|500+|10\.000+|Unique players login trong 24h|
|D1 Retention|> 40%|> 55%|% người chơi ngày 1 quay lại ngày 2|
|D7 Retention|> 20%|> 30%|% người chơi quay lại sau 7 ngày|
|Session Duration|> 30 phút|> 60 phút|Thời gian trung bình mỗi lần chơi|
|Zones per Session|> 1.5 zone|> 2.5 zone|Số zone khác nhau người chơi ghé thăm|
|ARPU (tháng)|> 20.000 VND|> 80.000 VND|Doanh thu / số người chơi tháng đó|
|Vietnam Pass Conversion|N/A|> 5%|% người chơi mua Vietnam Pass|
## **28.4. Privacy & Dữ Liệu Người Chơi**
- Tất cả analytics data được anonymize: không track tên thật, chỉ user\_id ngẫu nhiên
- Opt-out: người chơi có thể tắt analytics trong Settings, ngoài error reporting cơ bản
- Data retention: raw event data giữ 90 ngày, aggregated data giữ 2 năm
- Tuân thủ Nghị định 13/2023/NĐ-CP và GDPR cho người chơi quốc tế


# **29. Accessibility — Khả Năng Tiếp Cận**
Accessibility không phải tính năng thêm vào sau — phải được thiết kế từ đầu. Mục tiêu: Vietnam Online chơi được với mọi người, kể cả người có khiếm khuyết thị giác, thính giác, vận động hoặc nhận thức.
## **29.1. Thị Giác (Visual Accessibility)**

|**Tính năng**|**Mô tả**|**Ưu tiên**|
| :- | :- | :- |
|Text size scale|Tùy chỉnh 80%-200%, áp dụng toàn bộ UI không vỡ layout|P0|
|High contrast mode|Nền tối + chữ sáng, tăng contrast tất cả UI element|P1|
|Color blind modes|Protanopia, Deuteranopia, Tritanopia — điều chỉnh bảng màu HUD và marker bản đồ|P1|
|Subtitles / Captions|Tất cả dialog NPC và cutscene có phụ đề, tùy chỉnh size và background|P0|
|UI opacity|Điều chỉnh độ trong suốt HUD 0-100%|P2|
|Motion reduction|Giảm/tắt hiệu ứng camera shake, screen flash, particle dày đặc|P1|
|Dyslexia font|Tùy chọn font OpenDyslexic hoặc font sans-serif không chân thân thiện|P2|
## **29.2. Thính Giác (Audio Accessibility)**

|**Tính năng**|**Mô tả**|**Ưu tiên**|
| :- | :- | :- |
|Visual sound indicators|Icon hiển thị khi có âm thanh quan trọng (xe đang đến, người gọi bên cạnh)|P1|
|Mono audio mode|Kết hợp kênh trái-phải, người dùng 1 tai nghe không bị mất thông tin|P1|
|Volume controls riêng|Master / Music / SFX / Voice / Ambient — điều chỉnh độc lập|P0|
|Chat text thay voice|Mọi thông tin qua voice đều có phương án text thay thế|P0|
|Hearing loss mode|Tắt spatial audio, tăng volume SFX quan trọng, thêm visual cue|P2|
## **29.3. Vận Động (Motor Accessibility)**

|**Tính năng**|**Mô tả**|**Ưu tiên**|
| :- | :- | :- |
|Full keyboard remapping|Tất cả phím đều có thể đổi, save nhiều profile|P0|
|Gamepad full support|100% tính năng dùng được bằng gamepad, không cần chuột|P1|
|Auto-walk mode|Giữ phím hoặc tap đích đến, nhân vật tự đi — không cần giữ phím liên tục|P1|
|Hold vs Toggle|Tùy chọn: giữ phím để chạy vs bật/tắt toggle|P1|
|Click-to-move|Tap/click vị trí trên minimap → nhân vật tự di chuyển đến đó|P2|
|One-hand mode|Layout phím một tay cho người dùng chỉ dùng được một bên|P2|
|Slow cursor sensitivity|Tùy chỉnh độ nhạy chuột/joystick rộng (0.1x – 5x)|P0|
## **29.4. Nhận Thức (Cognitive Accessibility)**
- Không có time pressure trong hầu hết quest — người chơi làm theo tốc độ của mình
- Hướng dẫn rõ ràng, chia nhỏ: không dump thông tin 1 lần, hiện từng bước
- Undo / Confirm: mọi hành động quan trọng (xóa, mua, bán lớn) đều có confirm dialog
- Consistent UI: cùng hành động luôn ở cùng vị trí, không thay đổi layout đột ngột
- Reading mode: NPC dialog hiện đầy đủ text, không bắt buộc đọc kịp auto-scroll
- Difficulty không phải combat: không có khái niệm 'quá khó', mọi người đều khám phá theo tốc độ riêng
## **29.5. Checklist Accessibility trước Ra Mắt**
- Test với người dùng mù màu thực tế (3 loại phổ biến)
- Test full gameplay chỉ bằng keyboard, không dùng chuột
- Test full gameplay chỉ bằng gamepad
- Tất cả text UI pass WCAG AA contrast ratio
- Subtitle khả dụng cho 100% dialog có âm thanh
- Không có thông tin chỉ truyền qua màu sắc mà không có ký hiệu/text kèm theo


# **30. Audio Design — Thiết Kế Âm Thanh**
Âm thanh trong Vietnam Online không chỉ là nền nhạc — nó là ngôn ngữ thứ hai của thế giới ảo. Tiếng mưa trên mái tôn, tiếng xe máy hẻm nhỏ, tiếng cồng chiêng xa xa Tây Nguyên — đây là những gì làm người chơi cảm thấy đang ở Việt Nam thật.
## **30.1. Kiến Trúc Audio Engine**

|**Layer**|**Công nghệ**|**Mô tả**|
| :- | :- | :- |
|Audio Engine|Unreal Engine 5 — MetaSounds|Hệ thống audio procedural, graph-based, linh hoạt hơn WAV tĩnh|
|Spatial Audio|UE5 Spatial Audio + Occlusion|Âm thanh có vị trí 3D, bị chặn bởi tường, giảm theo khoảng cách thực tế|
|Ambient System|UE5 Audio Volumes + Zone-based|Mỗi khu vực có ambient soundscape riêng, blend mượt khi di chuyển|
|Music System|Adaptive MIDI + recorded tracks|Nhạc thay đổi tempo/layer theo trạng thái (đi bộ yên tĩnh / xe máy nhanh / lễ hội)|
|Voice System|Recorded VO + TTS backup|NPC Layer 3 có voice actor, Layer 2 dùng TTS tiếng Việt chất lượng cao|
|Mixing|Dynamic mixing với Unreal Audio Mixer|Tự động ducking: ambient nhỏ lại khi có dialog quan trọng|
## **30.2. Soundscape Library — Thư Viện Âm Thanh**

|**Category**|**Số lượng mục tiêu**|**Ví dụ tiêu biểu**|
| :- | :- | :- |
|Ambient vùng|30+ soundscapes|Phố Cổ Hà Nội sáng / Chợ trưa / Biển chiều / Tây Nguyên đêm|
|Weather SFX|20+ variations|Mưa nhẹ / Mưa lớn / Bão / Sương / Gió biển / Gió núi|
|Vehicle SFX|40+ per vehicle type|Xe máy Honda Win / SH / xe đạp / xích lô / xe buýt / tàu hỏa|
|Footstep materials|15+ materials|Đường nhựa / đá cuội / cát biển / gỗ nhà sàn / gạch chùa / sàn khách sạn|
|NPC Crowds|10+ density levels|Vắng (5 người) đến đông đúc (chợ trưa, lễ hội)|
|Food & Market SFX|50+ sounds|Tiếng thái rau, tiếng rang mực, tiếng pha cà phê phin nhỏ giọt|
|Nature SFX|60+ sounds|Chim kêu theo vùng, côn trùng đêm, sóng biển, gió thông Đà Lạt|
|UI SFX|30+ sounds|Button click, notification, quest complete, level up — phong cách tự nhiên, không game-y|
## **30.3. Âm Nhạc Nền (Original Soundtrack)**

|**Track type**|**Mô tả**|**Nhạc cụ chủ đạo**|
| :- | :- | :- |
|Main Theme|Chủ đề nhận ra được của Vietnam Online, 2-3 phút, orchestral|Đàn tranh solo, sau đó full orchestra nhẹ|
|Hanoi Theme|Cổ kính, chiều sâu, nhịp chậm|Đàn bầu + piano nhẹ + strings|
|Hoi An Theme|Thơ mộng, ấm áp, vui tươi|Sáo trúc + đàn tranh + acoustic guitar|
|Da Nang Theme|Tươi sáng, năng động, biển cả|Percussion nhẹ + brass + flute|
|Tay Nguyen Theme|Hùng tráng, nguyên sơ, bí ẩn|Cồng chiêng + t'rưng + bass drum|
|Da Lat Theme|Lãng mạn, se lạnh, mơ màng|Piano + violin + clarinet nhẹ|
|Saigon Night Theme|Sôi động, hiện đại, hỗn hợp|Lo-fi hip hop beats + đàn tranh sample|
|Festival Theme|Vui vẻ, náo nhiệt, celebrations|Full percussion + brass + đàn tranh nhanh|
|Calm Exploration|Ambient nhẹ, không gây mất tập trung|Ambient pad + water sounds + thỉnh thoảng đàn bầu|
## **30.4. Adaptive Music System**
Nhạc không phát ngẫu nhiên — nó phản ứng với trạng thái gameplay:

- Đi bộ khám phá: nhạc ambient nhẹ, 1-2 nhạc cụ, giúp tập trung vào thế giới
- Đi xe máy nhanh: nhạc tăng tempo, thêm percussion, cảm giác tốc độ
- Tham quan địa điểm lịch sử: nhạc chuyển sang trang nghiêm, đàn tranh cổ điển
- Lễ hội đang diễn ra: nhạc festival override, phát rõ trong bán kính 200m
- Gặp gỡ bạn bè, party: nhạc ấm áp, vui tươi hơn một chút
- Đêm khuya, vắng vẻ: nhạc minimal, cricket ambient, không áp lực
## **30.5. Voice Over & Localization Audio**

|**NPC Layer**|**Voice approach**|**Chi phí**|
| :- | :- | :- |
|Layer 1 — NPC nền|Không có voice, chỉ ambient crowd murmur tổng hợp|Gần 0 — procedural|
|Layer 2 — NPC quan trọng|TTS chất lượng cao (tiếng Việt vùng miền đúng giọng)|~$0.01/câu với dịch vụ TTS cloud|
|Layer 3 — NPC đặc biệt|Voice actor người thật, script viết tay, thu âm studio|~$500-2000/NPC cho bộ đầy đủ|
|UI / System messages|TTS thống nhất, giọng trung tính dễ nghe|TTS cloud|
|Tutorial narrator|Voice actor chuyên nghiệp, cả tiếng Việt lẫn tiếng Anh|Ưu tiên cao, thu sớm|
## **30.6. Audio Production Pipeline**

|**Bước**|**Task**|**Tool**|**Tiêu chuẩn kỹ thuật**|
| :- | :- | :- | :- |
|1\. Field Recording|Thu âm thực địa tại Việt Nam: chợ, phố, thiên nhiên|Zoom H6 / Sound Devices MixPre|24-bit / 96kHz, WAV uncompressed|
|2\. Studio Recording|Thu âm voice actor, nhạc cụ dân tộc|Pro Tools / Reaper|24-bit / 48kHz, phòng cách âm|
|3\. Editing & Cleaning|Lọc noise, normalize, edit timing|iZotope RX + DAW|SNR > 60dB, headroom -3dBFS|
|4\. Music Composition|Sáng tác + arrange theo style guide|Logic Pro / Cubase|Stems riêng biệt để adaptive mixing|
|5\. Integration|Import vào UE5 MetaSounds, cài spatial, loop point|Unreal Engine 5|Compressed: OGG Vorbis q6 cho game|
|6\. Mix & Master|Balance tổng thể, test trên nhiều thiết bị|UE5 Audio Mixer + external DAW|LUFS -14 cho music, -18 cho ambient|
|7\. QA Audio|Test trên headphone, loa, mobile, gamepad haptic|Manual testing|Không clip, không gap, loop mượt|



**— Hết Tài Liệu Bổ Sung —**

*Vietnam Online Project Documentation — Chương 19–30 — v1.0*
Trang  / 
