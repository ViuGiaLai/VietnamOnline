**VIETNAM ONLINE — Tài Liệu Dự Án**

**VIETNAM ONLINE**

*Thế Giới Du Lịch Ảo Toàn Quốc Việt Nam*

Tài liệu phân tích & thiết kế dự án — v1.0


# **1. Tổng Quan Dự Án**


Vietnam Online là một thế giới ảo mô phỏng Việt Nam, nơi người chơi có thể du lịch, gặp gỡ người khác, khám phá văn hóa và trải nghiệm địa điểm thực tế theo cách nhập vai trực tuyến nhiều người chơi (MMO).

**Điểm khác biệt cốt lõi của dự án:**

- Không phải bản đồ 3D đơn thuần như Google Earth.
- Không phải game hành động, đánh quái hay bắn súng.
- Là trải nghiệm sống: du lịch, xã hội, giáo dục, văn hóa, khám phá.

Người chơi chọn bất kỳ địa điểm nào ở Việt Nam — Hà Nội, Đà Nẵng, TP.HCM, Hội An, Đà Lạt, Gia Lai, Nha Trang, v.v. — và trải nghiệm như một khách du lịch thật, gặp người chơi khác, nói chuyện với NPC AI và khám phá văn hóa địa phương.

# **2. Lựa Chọn Ngôn Ngữ & Công Nghệ**


## **2.1. Ngôn Ngữ Lập Trình**

|**Ngôn ngữ**|**Ưu điểm**|**Nhược điểm**|**Phù hợp dự án?**|
| :- | :- | :- | :- |
|C++|Nền tảng vững, dùng với Unreal Engine, hiệu năng cao, nhiều việc làm|Khó học hơn, dài hơn|Rất phù hợp|
|C#|Phổ biến với Unity, dễ viết code UI|Unity ít mạnh hơn Unreal cho thế giới lớn|Phù hợp (thay thế)|
|Rust|An toàn bộ nhớ, hiệu năng cao|Ít việc làm game, cộng đồng nhỏ hơn|Không ưu tiên|
|C (thuần)|Học nền tảng|Quá thấp tầng, ít công ty tuyển|Chỉ để học|

**Kết luận: Chọn C++ + Unreal Engine là lộ trình tối ưu cho dự án quy mô lớn, thế giới 3D và multiplayer.**

## **2.2. Stack Công Nghệ Đề Xuất**

|**Thành phần**|**Công nghệ**|**Lý do chọn**|
| :- | :- | :- |
|Client (Game)|C++ + Unreal Engine|3D đẹp, multiplayer mạnh, hỗ trợ thế giới lớn|
|Server game|C++ hoặc Go|Hiệu năng cao, xử lý nhiều kết nối đồng thời|
|Database|PostgreSQL (Supabase)|Miễn phí, mạnh, quen thuộc, lưu dữ liệu người chơi|
|Bản đồ|OpenStreetMap + Cesium for Unreal|Miễn phí, dữ liệu thực tế toàn Việt Nam|
|Đăng nhập / Friends|Epic Online Services|Miễn phí, tích hợp sẵn với Unreal|
|AI NPC|Mô hình ngôn ngữ (tích hợp sau)|Giai đoạn sau khi có nền tảng ổn định|
|Voice Chat|Tích hợp sau (giai đoạn 3+)|Ưu tiên thấp ban đầu|

## **2.3. Nền Tảng (Platform)**

**PC trước — Mobile sau:**

- PC (Windows) là nền tảng đầu tiên vì thế giới 3D lớn, nhiều NPC, multiplayer đòi hỏi sức mạnh phần cứng.
- Mobile bổ sung sau khi PC ổn định (giai đoạn 2-3), cùng tài khoản và server.
- Mục tiêu dài hạn: Cross-platform (PC + Android + iOS) cùng một thế giới.

# **3. Kiến Trúc Thế Giới & Bản Đồ**


## **3.1. Cấu Trúc Thế Giới**

Không làm từng tỉnh riêng lẻ. Xây dựng một nền tảng thống nhất — Vietnam Online — với các vùng (zone) là các địa điểm thực tế:

|**Vùng (Zone)**|**Địa điểm tiêu biểu**|**Ưu tiên triển khai**|
| :- | :- | :- |
|Hà Nội|Hồ Gươm, Phố Cổ, Lăng Bác|Giai đoạn 2|
|TP.HCM|Bến Thành, Phố đi bộ Nguyễn Huệ|Giai đoạn 2|
|Đà Nẵng|Bãi biển Mỹ Khê, Golden Bridge|Giai đoạn 1-2|
|Hội An|Phố Cổ, đèn lồng, chùa Cầu|Giai đoạn 1 (MVP)|
|Đà Lạt|Hồ Xuân Hương, chợ đêm|Giai đoạn 2|
|Nha Trang|Bãi biển, Vinpearl|Giai đoạn 3|
|Gia Lai|Biển Hồ, Chư Đăng Ya, Pleiku|Giai đoạn 3|
|Các tỉnh khác|Huế, Cần Thơ, Hải Phòng...|Giai đoạn 4+|

## **3.2. Kỹ Thuật Bản Đồ**

**Nguồn dữ liệu bản đồ:**

- OpenStreetMap: đường sá, tòa nhà, nhà hàng, khách sạn, ATM, bệnh viện — miễn phí.
- Cesium for Unreal: tích hợp bản đồ Trái Đất 3D trực tiếp vào Unreal Engine.
- Overpass API: truy vấn dữ liệu OSM theo khu vực (ví dụ: toàn bộ nhà hàng Hội An).
- Mapzen Terrain Tiles: dữ liệu địa hình (núi, đồi, sông, biển).

**Thuật toán kỹ thuật quan trọng:**

|**Thuật toán / Kỹ thuật**|**Mô tả**|**Ứng dụng**|
| :- | :- | :- |
|Spatial Partitioning|Chia bản đồ thành ô lưới, chỉ tải vùng xung quanh người chơi|Thế giới lớn, tiết kiệm RAM/GPU|
|Level of Detail (LOD)|Mô hình xa = ít polygon, gần = nhiều polygon|Tăng FPS, tối ưu render|
|A\* Pathfinding|f(x)=g(x)+h(x) — tìm đường ngắn nhất cho NPC|NPC di chuyển thực tế|
|Navigation Mesh|Lưới điều hướng cho NPC đi lại trong không gian 3D|NPC tránh chướng ngại vật|
|Streaming / LOD Zones|Tải động từng khu vực khi người chơi tiếp cận|Hàng trăm địa điểm mà không lag|

# **4. Hệ Thống Trong Game**


## **4.1. Hệ Thống Di Chuyển (Đầy Đủ)**

|**Phương tiện**|**Chi tiết**|**Độ khó xây dựng**|
| :- | :- | :- |
|Đi bộ / Chạy|Di chuyển tự do, xem bản đồ, hỏi đường|Thấp|
|Xe đạp|Thuê theo giờ, trả xe tại trạm|Thấp|
|Xe máy|Thuê theo giờ/ngày, đổ xăng, chọn loại xe|Trung bình|
|Ô tô tự lái|Thuê theo ngày, lái trong thành phố|Trung bình|
|Taxi / Xe công nghệ|Gọi xe, đặt trước, chia sẻ chuyến|Trung bình|
|Xe buýt|Xem tuyến, mua vé, chờ xe tại trạm|Trung bình|
|Xe khách liên tỉnh|Đặt vé, chọn ghế, hành trình dài|Cao|
|Tàu hỏa|Đặt vé, chọn toa/khoang, theo lịch|Cao|
|Máy bay|Check-in, qua cổng, bay giữa các thành phố|Cao|
|Tàu thủy / Cano|Du lịch biển đảo, sông nước miền Tây|Trung bình|
|Cáp treo|Khu du lịch núi, công viên chủ đề|Thấp|

## **4.2. Hệ Thống Lưu Trú**

- Khách sạn: đặt phòng, nhận phòng, trả phòng, đánh giá.
- Homestay: tương tác với chủ nhà, trải nghiệm bản địa.
- Resort: dịch vụ cao cấp, hồ bơi, spa.
- Hostel: phòng tập thể, gặp gỡ người chơi khác.
- Camping: cắm trại tại các điểm du lịch thiên nhiên.

## **4.3. Hệ Thống Ăn Uống**

- Nhà hàng, quán ăn đường phố, quán cà phê, chợ đêm.
- Đặc sản địa phương theo từng vùng: Phở (Hà Nội), Bánh mì (TP.HCM), Bún bò (Huế), Cơm lam (Gia Lai).
- Gọi món, thanh toán, đánh giá quán.

## **4.4. Hệ Thống Tham Quan**

- Bảo tàng, chùa, nhà thờ, di tích lịch sử, bãi biển, núi, công viên, khu vui chơi.
- Mỗi địa điểm có: tên thật, vị trí GPS, ảnh thực, thông tin lịch sử, giờ mở cửa.
- Check-in, sưu tầm huy hiệu, lưu ảnh trong game.

## **4.5. Hệ Thống Xã Hội & Người Chơi**

- Kết bạn, lập nhóm du lịch, chat text và voice chat (giai đoạn 3+).
- Chụp ảnh chung, chia sẻ hành trình, tổ chức sự kiện.
- Hệ thống thành tựu: đến 63 tỉnh, ăn 100 món, gặp 500 người chơi.

## **4.6. Nghề Nghiệp Người Chơi**

|**Nghề nghiệp**|**Hoạt động chính**|**Thu nhập trong game**|
| :- | :- | :- |
|Khách du lịch|Khám phá, check-in, sưu tầm huy hiệu|Từ nhiệm vụ|
|Hướng dẫn viên|Dẫn tour, giới thiệu địa điểm, tổ chức đoàn|Phí tour từ người chơi khác|
|Nhiếp ảnh gia|Chụp ảnh, bán ảnh cho người chơi và hệ thống|Bán ảnh|
|Tài xế|Chở khách trong game, dịch vụ taxi nội bộ|Phí chuyến đi|
|Chủ khách sạn|Quản lý phòng, đặt phòng, tiếp đón|Phí lưu trú|
|Chủ nhà hàng|Quản lý thực đơn, phục vụ khách|Doanh thu nhà hàng|
|Người tổ chức tour|Lập kế hoạch, quảng cáo tour, quản lý nhóm|Phí tổ chức|

## **4.7. Hệ Thống Kinh Tế**

- Tiền trong game dùng để: thuê xe, mua vé tham quan, đặt khách sạn, mua đặc sản, đồ lưu niệm.
- Thu nhập từ nghề nghiệp, hoàn thành nhiệm vụ, bán ảnh/nội dung.
- Mua sắm: chợ, siêu thị, trung tâm thương mại, cửa hàng lưu niệm.

## **4.8. Hệ Thống Thời Tiết & Văn Hóa**

- Thời tiết đồng bộ thực tế (nắng, mưa, bão, sương mù) theo từng địa điểm.
- Lễ hội địa phương: Festival Huế, Tết Nguyên Đán, lễ hội Tây Nguyên.
- Trò chơi dân gian, nghệ thuật truyền thống, âm nhạc bản địa.

# **5. Hệ Thống NPC AI**


## **5.1. Vấn Đề Kỹ Thuật**

|**Vấn đề**|**Mô tả**|**Giải pháp**|
| :- | :- | :- |
|Chi phí AI|1000 NPC × 1 cuộc/phút = 60.000 API call/giờ|Phân cấp NPC, chỉ NPC quan trọng mới dùng AI đầy đủ|
|NPC vô nghĩa|Hội thoại lặp, thiếu ngữ cảnh thế giới|Cung cấp context: giờ, địa điểm, nghề nghiệp, thời tiết|
|Trí nhớ|NPC quên những gì đã nói|Xây hệ thống memory riêng, lưu ngắn hạn và dài hạn|
|Hiệu năng|10\.000 NPC hoạt động đồng thời = sập server|Chỉ kích hoạt AI khi người chơi ở gần|
|NPC hội thoại với nhau|Chi phí và độ phức tạp cao|Dùng Social Simulation thay vì AI call cho mọi cặp NPC|

## **5.2. Phân Cấp NPC (3 Lớp)**

|**Lớp**|**Loại NPC**|**AI**|**Số lượng**|
| :- | :- | :- | :- |
|Layer 1|NPC nền (đi lại, hoạt động sinh hoạt)|Không AI — lịch trình cố định|~99.000|
|Layer 2|NPC quan trọng (chủ quán, tài xế, bảo vệ)|AI nhẹ — trả lời cơ bản|~900|
|Layer 3|NPC đặc biệt (hướng dẫn viên, nhân vật chính)|AI đầy đủ — hội thoại tự nhiên|~100|

## **5.3. Cuộc Sống NPC (Social Simulation)**

Mỗi NPC Layer 2-3 có đầy đủ thuộc tính:

- Thông tin: Tên, tuổi, nghề nghiệp, nơi ở, nơi làm việc, sở thích, quan hệ xã hội.
- Lịch sinh hoạt: 6h tập thể dục → 8h đi làm → 12h ăn trưa → 18h về nhà → 20h giải trí.
- Trí nhớ: Nhớ người chơi đã gặp, các cuộc hội thoại gần đây, sự kiện quan trọng.
- NPC nói chuyện với nhau: Giả lập qua Social Simulation, không phải AI call thực sự.

**Ví dụ trả lời thông minh:**

Người chơi hỏi: "Quán bún bò gần đây ở đâu?"

NPC biết: giờ hiện tại (22h), vị trí (Đà Nẵng), thời tiết (mưa) → trả lời: "Giờ này hầu hết quán đóng rồi, nhưng có quán 24h ở đầu đường Trần Phú đó."

# **6. Hệ Thống Multiplayer & Server**


## **6.1. Vấn Đề Kỹ Thuật Multiplayer**

|**Vấn đề**|**Mô tả**|**Giải pháp**|
| :- | :- | :- |
|Đồng bộ vị trí|Server cập nhật vị trí hàng ngàn người liên tục|Interest management, chỉ gửi data những người gần nhau|
|Sharding / Instancing|Không thể để 10.000 người đứng cùng một con đường|Chia kênh (instance), tối đa ~200 người/kênh|
|Chống gian lận|Hack tiền, teleport, tốc độ bất thường|Logic quan trọng chạy trên server, client chỉ nhận kết quả|
|Máy chủ|Cần login server, game server, database, chat server|Bắt đầu với Oracle Cloud Free Tier|

## **6.2. Kiến Trúc Server**

**Giai đoạn 1 (thử nghiệm):**

- Game Server (chạy trên PC cá nhân hoặc Oracle Cloud Free)
- Database: Supabase (PostgreSQL) — miễn phí
- Đăng nhập & Friends: Epic Online Services — miễn phí

**Giai đoạn 3+ (mở rộng):**

- Gateway → World Server → AI Server → Voice Server → Database Cluster

## **6.3. Công Cụ Free Cho Giai Đoạn Đầu**

|**Dịch vụ**|**Chức năng**|**Chi phí**|
| :- | :- | :- |
|Oracle Cloud Free Tier|VPS server chạy game server|Miễn phí (mãi mãi với giới hạn)|
|Supabase|Database PostgreSQL, Auth, Storage|Miễn phí tier đủ dùng giai đoạn đầu|
|Epic Online Services|Đăng nhập, bạn bè, matchmaking|Miễn phí|
|Cesium for Unreal|Bản đồ 3D tích hợp Unreal Engine|Miễn phí (mã nguồn mở)|
|OpenStreetMap + Overpass|Dữ liệu địa điểm, đường sá, POI|Miễn phí hoàn toàn|
|PC cá nhân (server thử nghiệm)|Bạn bè kết nối qua IP/VPN|0 đồng|

**Chi phí thực tế ban đầu: 0 - 5 USD/tháng với vài người chơi thử nghiệm.**

# **7. Nguồn Dữ Liệu & GitHub Đáng Tham Khảo**


|**Tên**|**Link**|**Dùng để làm gì**|
| :- | :- | :- |
|OpenStreetMap|openstreetmap.org|Bản đồ, đường sá, địa điểm toàn Việt Nam — miễn phí|
|CesiumJS (GitHub)|github.com/CesiumGS/cesium|Render bản đồ Trái Đất 3D trong trình duyệt|
|Cesium for Unreal (GitHub)|github.com/CesiumGS/cesium-unreal|Tích hợp bản đồ 3D vào Unreal Engine — QUAN TRỌNG NHẤT|
|MapLibre GL JS (GitHub)|github.com/maplibre/maplibre-gl-js|Thay thế Google Maps mã nguồn mở|
|Overpass Turbo|overpass-turbo.eu|Truy vấn dữ liệu OSM theo khu vực cụ thể|
|Mapzen Terrain Tiles (GitHub)|github.com/tilezen/joerd|Dữ liệu địa hình núi, đồi, sông, biển|

**Combo đề xuất cho Vietnam Online:**

OpenStreetMap + Cesium for Unreal + Unreal Engine + C++ → Có thể tạo nguyên mẫu Việt Nam 3D với đường phố thật, nhà hàng thật, người chơi đi lại, gặp nhau online — trong vài tháng học tập.

# **8. Lộ Trình Phát Triển**


## **8.1. MVP — Phiên Bản 0.1 (Mục tiêu đầu tiên)**

Chỉ 1 thành phố (đề xuất: Hội An hoặc Đà Nẵng):

- Đăng nhập tài khoản
- Multiplayer 20-50 người cùng lúc
- Đi bộ tự do trong khu vực
- Chat text
- Thuê xe máy/xe đạp
- 20 địa điểm tham quan thật
- Hệ thống nhiệm vụ cơ bản
- Bản đồ mini

Chưa cần: AI NPC, thời tiết thật, 63 tỉnh, kinh tế phức tạp, voice chat, máy bay.

## **8.2. Lộ Trình Theo Phiên Bản**

|**Phiên bản**|**Thời gian (ước tính)**|**Tính năng chính**|
| :- | :- | :- |
|V1 — Foundation|Năm 1|1 thành phố, đi bộ, chat, thuê xe, 20 địa điểm, 20-50 người chơi|
|V2 — Life Sim|Năm 1-2|Khách sạn, nhà hàng, nhiệm vụ phong phú, 2-3 thành phố, 50-200 người|
|V3 — Economy|Năm 2|Nghề nghiệp, kinh tế, NPC AI cơ bản, voice chat, 5-8 thành phố|
|V4 — AI World|Năm 2-3|NPC AI thông minh, lễ hội, thời tiết thực, hàng trăm người chơi|
|V5 — Vietnam Full|Năm 3+|63 tỉnh thành, nội dung cộng đồng đóng góp, Mobile platform|

## **8.3. Nguyên Tắc Phát Triển**

- Không cố làm toàn bộ Việt Nam từ đầu — sẽ không bao giờ hoàn thành.
- Mỗi giai đoạn phải có sản phẩm chạy được, chơi được.
- Khi multiplayer ổn định mới thêm AI NPC, xe cộ, kinh tế.
- Cho phép cộng đồng đóng góp nội dung (ảnh, thông tin địa điểm) như Wikipedia/OSM.
- Bạn có lợi thế: hiểu văn hóa địa phương, có thể tự đi chụp ảnh, thu thập dữ liệu thực tế.

# **9. Thách Thức Lớn Nhất**


|**Thách thức**|**Mức độ**|**Ghi chú**|
| :- | :- | :- |
|Nội dung cho hàng trăm địa điểm|Rất cao|Giải pháp: cộng đồng đóng góp, không tự làm tất cả|
|Mô hình 3D toàn Việt Nam|Rất cao|Dùng Cesium + OSM + procedural generation|
|Dung lượng (hàng chục/trăm GB)|Cao|Streaming dữ liệu theo khu vực|
|Dữ liệu thay đổi liên tục|Cao|Cập nhật từ OSM, cộng đồng báo cáo|
|NPC hội thoại tự nhiên và thông minh|Cao|Social Simulation + AI phân cấp|
|Pháp lý (tên/logo doanh nghiệp thật)|Trung bình|Cần xem xét quyền sử dụng dữ liệu thương hiệu|
|Chi phí server khi mở rộng|Trung bình|Tăng dần theo số người chơi, có thể monetize|

# **10. Tầm Nhìn & Định Vị Sản Phẩm**


**Vietnam Online không cạnh tranh với Google Maps hay Google Earth. Thứ người dùng chưa có là:**

|**Google Maps / Earth có**|**Vietnam Online có**|
| :- | :- |
|Xem địa điểm từ xa|Sống trong địa điểm như người thật|
|Ảnh và thông tin tĩnh|Trải nghiệm động: đi, ăn, gặp gỡ, thuê xe|
|Một mình dùng|Xã hội: gặp người chơi khác từ khắp thế giới|
|Không có nhân vật|Nhập vai: chọn nghề, kiếm tiền, dẫn tour|
|Không AI nói chuyện|NPC AI kể chuyện, hướng dẫn, tương tác|

**Tiềm năng dự án có thể phát triển thành:**

- Game du lịch: người chơi khám phá Việt Nam từ nước ngoài.
- Nền tảng du lịch ảo: trải nghiệm trước khi đi thật.
- Công cụ học tiếng Anh/Việt: giao tiếp với NPC và người chơi quốc tế.
- Công cụ quảng bá du lịch Việt Nam ra thế giới.
- Nền tảng cộng đồng: tài liệu hóa văn hóa địa phương 63 tỉnh thành.

*Đây là ý tưởng có quy mô gần với một startup công nghệ hơn là game cá nhân thông thường — nhưng hoàn toàn khả thi nếu phát triển đúng lộ trình, bắt đầu từ nhỏ và mở rộng dần.*


# **11. Hệ Thống Người Chơi (Player Systems)**


## **11.1. Hồ Sơ Nhân Vật**

Mỗi người chơi có một nhân vật (avatar) đại diện trong thế giới Vietnam Online với đầy đủ thông tin cá nhân hóa:

|**Thuộc tính**|**Mô tả**|**Ghi chú**|
| :- | :- | :- |
|Tên hiển thị|Tên người chơi trong game|Có thể đặt tên Việt Nam hoặc quốc tế|
|Avatar 3D|Ngoại hình nhân vật tùy chỉnh|Chọn tóc, da, quần áo, phụ kiện|
|Quốc tịch|Quốc gia xuất phát của nhân vật|Ảnh hưởng đến dialogue với NPC|
|Nghề nghiệp|Vai trò trong game (du khách, HDV…)|Có thể đổi sau|
|Cấp độ / EXP|Kinh nghiệm tích lũy qua hành trình|Tăng qua nhiệm vụ, khám phá|
|Danh hiệu|Ví dụ: "Nhà Thám Hiểm Hội An"|Mở khóa khi hoàn thành thành tựu|
|Nhật ký hành trình|Lưu lịch sử địa điểm đã đến|Có thể chia sẻ công khai|

## **11.2. Hệ Thống Cấp Độ & Kỹ Năng**

|**Kỹ năng**|**Cách tăng**|**Hiệu quả**|
| :- | :- | :- |
|Giao tiếp|Nói chuyện với NPC, tham gia tour nhóm|NPC thân thiện hơn, giảm giá dịch vụ|
|Lái xe|Thuê và di chuyển bằng xe máy, ô tô|Lái nhanh hơn, mở xe cao cấp hơn|
|Nhiếp ảnh|Chụp ảnh, nhận đánh giá từ cộng đồng|Ảnh chất lượng cao, bán giá tốt hơn|
|Nấu ăn|Tham gia workshop ẩm thực địa phương|Mở khóa recipe, mở quán ăn|
|Lịch sử & Văn hóa|Tham quan bảo tàng, nghe thuyết minh|Mở thông tin ẩn, nhiệm vụ đặc biệt|
|Ngôn ngữ|Giao tiếp với NPC, người chơi quốc tế|Hiểu tiếng địa phương, hội thoại phong phú hơn|
|Kinh doanh|Mở và điều hành cơ sở trong game|Tăng doanh thu, mở tính năng quản lý|

## **11.3. Hệ Thống Danh Tiếng (Reputation)**

Mỗi người chơi có chỉ số danh tiếng tại từng địa điểm trong game:

- Danh tiếng cao: NPC chào đón nồng nhiệt hơn, giảm giá dịch vụ, mở nhiệm vụ đặc biệt.
- Danh tiếng thấp: NPC cảnh giác, bị từ chối một số dịch vụ, cần "sửa chữa" bằng hành động tích cực.
- Danh tiếng theo vùng: nổi tiếng ở Hội An nhưng chưa ai biết ở Hà Nội.
- Danh tiếng nghề nghiệp: hướng dẫn viên có rating từ người chơi khác (kiểu Grab/Airbnb).

## **11.4. Hệ Thống Bạn Bè & Nhóm**

- Kết bạn trong game, theo dõi hành trình của nhau.
- Tạo nhóm du lịch (Party): cùng di chuyển, chia sẻ bản đồ, mục tiêu chung.
- Guild / Hội du lịch: tổ chức lớn hơn, tổ chức sự kiện, giải đấu.
- Hệ thống chia sẻ: đăng ảnh, review địa điểm lên feed chung.
- Chat riêng tư, chat nhóm, chat vùng (proximity voice chat — giai đoạn 3).

## **11.5. Hệ Thống Thành Tựu & Bộ Sưu Tập**

|**Danh mục thành tựu**|**Ví dụ cụ thể**|**Phần thưởng**|
| :- | :- | :- |
|Địa lý|Đặt chân đến 63 tỉnh thành|Danh hiệu "Người Việt Nam thu nhỏ"|
|Ẩm thực|Thử 200 món ăn khác nhau|Mở khóa NPC đầu bếp đặc biệt|
|Giao thông|Đi tất cả 11 loại phương tiện|Huy hiệu "Người Lữ Hành"|
|Xã hội|Kết bạn với 500 người chơi|Skin avatar độc quyền|
|Hướng dẫn viên|Dẫn 100 tour thành công|Danh hiệu "Hướng Dẫn Viên 5 Sao"|
|Nhiếp ảnh|100 ảnh được cộng đồng yêu thích|Triển lãm ảo trong game|
|Lịch sử|Hoàn thành quiz về 50 di tích|Mở bản đồ cổ Việt Nam|

## **11.6. Hệ Thống Tùy Chỉnh Avatar**

- Trang phục: áo dài, quần áo du lịch, trang phục dân tộc (Tây Nguyên, Mông, Dao…).
- Phụ kiện: ba lô, máy ảnh, nón lá, kính, đồ lưu niệm mang trên người.
- Biểu cảm (emote): vẫy tay, cúi chào, chụp ảnh selfie, nhảy múa.
- Khung hình / Border profile: đạt được qua thành tựu, sự kiện đặc biệt.


# **12. Hệ Thống Nội Dung Do Cộng Đồng Tạo (UGC)**


Đây là yếu tố sống còn của dự án. Không thể một nhóm nhỏ tự xây dựng nội dung cho 63 tỉnh thành. Cộng đồng người dùng sẽ là "lực lượng biên tập" khổng lồ — giống cách Wikipedia và OpenStreetMap vận hành.

## **12.1. Các Loại Nội Dung Cộng Đồng Đóng Góp**

|**Loại nội dung**|**Ví dụ**|**Quy trình duyệt**|
| :- | :- | :- |
|Địa điểm mới|Quán cà phê ở Buôn Ma Thuột chưa có trong OSM|Kiểm duyệt tự động + cộng đồng vote|
|Ảnh thực tế|Ảnh Biển Hồ Gia Lai mùa khô vs mùa mưa|AI lọc nội dung + moderator|
|Thông tin lịch sử|Câu chuyện về Nhà Thờ Gỗ Kon Tum|Cộng đồng fact-check, nguồn dẫn|
|Menu nhà hàng|Danh sách món, giá, ảnh của quán địa phương|Chủ quán xác nhận hoặc cộng đồng|
|Tour tự tạo|Hành trình 3 ngày Gia Lai do người chơi thiết kế|Rating từ người tham gia|
|Sự kiện địa phương|Lễ hội đua thuyền, hội chợ, concert|Admin xác nhận + link nguồn|
|Câu chuyện dân gian|Truyền thuyết Hồ Tơ Nưng (Biển Hồ)|Cộng đồng kiểm duyệt|

## **12.2. Hệ Thống Tạo Tour (Tour Builder)**

Người chơi có thể tự thiết kế và bán tour cho người chơi khác:

- Chọn địa điểm, sắp xếp thứ tự, thêm mô tả và ảnh cho từng điểm dừng.
- Đặt giá tour (miễn phí hoặc trả phí bằng tiền game).
- Người tham gia tour nhận hướng dẫn trong game, NPC dẫn đường theo lịch trình.
- Sau tour, người tham gia rating và review — ảnh hưởng danh tiếng người tạo tour.
- Tour nổi bật được featured trên trang chủ.

## **12.3. Hệ Thống Kiểm Duyệt Nội Dung**

|**Cấp độ kiểm duyệt**|**Cơ chế**|**Thời gian xử lý**|
| :- | :- | :- |
|AI tự động (Lớp 1)|Lọc ảnh không phù hợp, spam, nội dung độc hại|Tức thì|
|Cộng đồng vote (Lớp 2)|Người dùng uy tín vote approve/reject|1-24 giờ|
|Moderator (Lớp 3)|Xem xét nội dung phức tạp, tranh chấp|1-3 ngày|
|Admin (Lớp 4)|Quyết định cuối cùng về nội dung nhạy cảm|Tùy tình huống|

## **12.4. Khuyến Khích Đóng Góp**

- Điểm UGC: mỗi lần đóng góp được duyệt → nhận điểm, quy đổi thành vật phẩm hoặc tiền game.
- Danh hiệu đặc biệt: "Đại Sứ Gia Lai", "Chuyên Gia Ẩm Thực Hà Nội".
- Bảng xếp hạng người đóng góp theo tỉnh.
- Ưu tiên hiển thị nội dung của người đóng góp nhiều.


# **13. Hệ Thống AI & Knowledge Graph**


Đây là "bộ não" của Vietnam Online. Thay vì mỗi NPC gọi riêng một AI model, toàn bộ hệ thống được xây dựng trên một Knowledge Graph tập trung về Việt Nam.

## **13.1. Knowledge Graph Việt Nam**

Một cơ sở tri thức khổng lồ về Việt Nam bao gồm:

|**Danh mục**|**Dữ liệu**|**Ví dụ**|
| :- | :- | :- |
|Địa lý|Tọa độ, ranh giới, địa hình, khí hậu|Gia Lai: cao nguyên, 800m so với mực nước biển|
|Lịch sử|Sự kiện, nhân vật, thời kỳ, di tích|Pleiku được thành lập năm 1929|
|Văn hóa|Lễ hội, phong tục, dân tộc, ngôn ngữ|Lễ hội đâm trâu của người Bahnar|
|Ẩm thực|Món ăn, nguyên liệu, công thức, quán nổi tiếng|Phở khô Gia Lai khác phở Hà Nội ở chỗ…|
|Kinh tế|Ngành nghề, sản phẩm đặc trưng, giá cả|Cà phê Arabica Gia Lai, giá thị trường|
|Người nổi tiếng|Nhân vật lịch sử, văn hóa, thể thao|Nhà thơ Kim Tuấn người Pleiku|
|Địa điểm|POI, rating, giờ mở cửa, giá vé|Biển Hồ Gia Lai: mở cửa 6h-18h, 50k/người|

## **13.2. Kiến Trúc AI NPC**

Mô hình hoạt động của AI NPC:

- Bước 1 — Nhận input: người chơi gõ hoặc nói câu hỏi.
- Bước 2 — Context extraction: hệ thống xác định giờ, vị trí, thời tiết, NPC đang nói chuyện là ai.
- Bước 3 — Knowledge Graph lookup: truy vấn thông tin liên quan từ cơ sở tri thức.
- Bước 4 — Personality filter: áp dụng tính cách, nghề nghiệp, trạng thái cảm xúc của NPC.
- Bước 5 — Memory check: kiểm tra NPC đã gặp người chơi này chưa, họ đã nói gì.
- Bước 6 — Generate response: tạo câu trả lời tự nhiên phù hợp ngữ cảnh.

## **13.3. Hệ Thống Memory NPC**

|**Loại memory**|**Lưu gì**|**Thời hạn**|
| :- | :- | :- |
|Short-term memory|Cuộc hội thoại hiện tại, bối cảnh gần đây|Trong session|
|Episode memory|Người chơi đã hỏi gì, đã đến đây khi nào|7-30 ngày|
|Long-term memory|Quan hệ đặc biệt, sự kiện đáng nhớ|Vĩnh viễn|
|World state memory|Sự kiện thế giới: lễ hội, thời tiết, tin tức local|Theo thời gian thực|

## **13.4. AI Tạo Nội Dung Tự Động**

- Procedural NPC dialogue: AI sinh câu thoại mới mỗi ngày, tránh lặp.
- Procedural quest generation: tạo nhiệm vụ ngẫu nhiên dựa trên vị trí và profile người chơi.
- AI mô hình 3D: dùng AI để sinh building facade, biển hiệu, texture phù hợp địa phương.
- AI dịch thuật: NPC có thể nói tiếng Anh, tiếng Việt, hoặc ngôn ngữ dân tộc địa phương.
- AI phân tích ảnh UGC: tự động gắn tag địa điểm, xác định nội dung phù hợp.

## **13.5. AI Gợi Ý Hành Trình**

Hệ thống recommendation engine cá nhân hóa:

- Dựa trên lịch sử di chuyển, sở thích, cấp kỹ năng → gợi ý địa điểm tiếp theo.
- Dựa trên người chơi cùng nhóm → gợi ý tour phù hợp cả nhóm.
- Dựa trên thời tiết thực + giờ trong ngày → gợi ý hoạt động hợp lý.
- Ví dụ: 15h chiều, trời nắng, ở Đà Nẵng → Gợi ý: "Bãi biển Mỹ Khê cách đây 2km, nước đẹp nhất lúc này."


# **14. Hệ Thống Vận Hành (LiveOps)**


LiveOps là toàn bộ hoạt động vận hành game sau khi ra mắt — bao gồm sự kiện, cập nhật, quản lý server và giữ chân người chơi. Đây là yếu tố quyết định game sống bao lâu.

## **14.1. Hệ Thống Sự Kiện Theo Mùa**

|**Thời điểm**|**Sự kiện**|**Nội dung đặc biệt**|
| :- | :- | :- |
|Tết Nguyên Đán (Jan-Feb)|Tết Vietnam Online|Pháo hoa, lì xì, chợ hoa, NPC mặc áo dài|
|30/4 - 1/5|Lễ hội Thống Nhất|Tour di tích lịch sử, nhiệm vụ đặc biệt|
|Mùa hè (Jun-Aug)|Festival Biển|Sự kiện tại Đà Nẵng, Nha Trang, Phú Quốc|
|Trung Thu (Sep-Oct)|Tết Trung Thu|Đèn lồng, múa lân, bánh trung thu|
|Giáng Sinh (Dec)|Christmas Vietnam|Trang trí đường phố, quà tặng đặc biệt|
|Ngày du lịch Việt Nam (9/7)|Vietnam Tourism Day|Mở địa điểm mới, giảm phí dịch vụ|

## **14.2. Hệ Thống Cập Nhật Nội Dung**

|**Chu kỳ**|**Loại cập nhật**|**Ví dụ**|
| :- | :- | :- |
|Hàng ngày|Thời tiết, giá cả, tin tức địa phương|Cập nhật thời tiết thực từ API|
|Hàng tuần|Sự kiện nhỏ, nhiệm vụ tuần|"Tuần lễ Cà phê Buôn Ma Thuột"|
|Hàng tháng|Địa điểm mới, NPC mới, tính năng nhỏ|Mở thêm 5 địa điểm Hà Nội|
|Hàng quý|Patch lớn, tỉnh mới, hệ thống mới|Ra mắt vùng Tây Bắc|
|Hàng năm|Expansion lớn, nâng cấp engine, tính năng major|Ra mắt Mobile platform|

## **14.3. Quản Lý & Giám Sát Server**

- Dashboard real-time: theo dõi số người chơi, khu vực đông nhất, tỷ lệ lỗi.
- Auto-scaling: tự động tăng server khi có đột biến người chơi (lễ hội, sự kiện).
- Health monitoring: cảnh báo khi server quá tải, tự restart khi cần.
- Log & Analytics: ghi lại hành vi người chơi để cải thiện gameplay.
- Backup: sao lưu database mỗi giờ, snapshot toàn bộ mỗi ngày.

## **14.4. Chống Toxicity & Vi Phạm**

|**Vi phạm**|**Phát hiện**|**Xử lý**|
| :- | :- | :- |
|Ngôn ngữ độc hại|AI lọc tự động + báo cáo người dùng|Cảnh cáo → Mute → Ban|
|Hack / Cheat|Server-side validation, phát hiện bất thường|Ban vĩnh viễn account + IP|
|Quấy rối người chơi|Hệ thống report + moderator review|Kick khỏi instance, hạn chế tính năng|
|Nội dung không phù hợp (UGC)|AI + moderator kiểm duyệt|Xóa nội dung, trừ điểm UGC|
|Spam / Quảng cáo|Phát hiện pattern, rate limiting|Mute chat, cảnh cáo|

## **14.5. Retention & Engagement**

- Daily login reward: nhận vật phẩm, tiền game mỗi ngày đăng nhập.
- Weekly challenge: nhiệm vụ tuần với phần thưởng độc quyền.
- Leaderboard: bảng xếp hạng theo địa điểm, thành tựu, UGC đóng góp.
- Push notification (Mobile): nhắc sự kiện sắp diễn ra, bạn bè đang online.
- Streak system: duy trì chuỗi đăng nhập, tăng phần thưởng theo số ngày liên tiếp.


# **15. Hệ Thống Kiếm Tiền (Monetization)**


Mô hình kiếm tiền phải cân bằng giữa doanh thu và trải nghiệm người chơi. Vietnam Online nên theo mô hình Free-to-Play với các nguồn thu bổ sung — không bán sức mạnh (Pay-to-Win).

## **15.1. Mô Hình Doanh Thu**

|**Nguồn thu**|**Mô tả**|**Ước tính tiềm năng**|
| :- | :- | :- |
|Cosmetic Shop|Trang phục, emote, skin avatar, khung hình — chỉ thẩm mỹ, không ảnh hưởng gameplay|Cao — người chơi sẵn sàng trả cho cá nhân hóa|
|Vietnam Pass (Premium)|Gói thuê bao tháng/năm: tăng tốc EXP, slot lưu ảnh nhiều hơn, tính năng UGC nâng cao|Trung bình — nguồn thu ổn định|
|Tour Marketplace|Hoa hồng 10-15% từ tour người chơi bán cho nhau|Trung bình — tăng theo lượng UGC|
|Quảng cáo du lịch thật|Sở du lịch, khách sạn, hãng hàng không trả tiền để hiển thị trong game|Cao — giá trị B2B lớn|
|Dữ liệu hành vi (ẩn danh)|Cung cấp insight xu hướng du lịch cho đơn vị lữ hành, nghiên cứu|Trung bình — cần tuân thủ pháp lý|
|API / Licensing|Cho phép bên thứ ba dùng Knowledge Graph Việt Nam|Dài hạn — tiềm năng lớn|
|Event Sponsorship|Thương hiệu tài trợ lễ hội trong game (Tết, mùa hè)|Trung bình — theo mùa|

## **15.2. Cosmetic Shop — Chi Tiết**

- Trang phục địa phương: áo dài 3 miền, trang phục dân tộc Tây Nguyên, Tây Bắc, Nam Bộ.
- Phương tiện đặc biệt: xe máy vintage, xe đạp cổ, xích lô phiên bản giới hạn.
- Emote văn hóa: vẫy nón lá, múa sạp, giả lập đánh cồng chiêng.
- Khung hình sự kiện: chỉ có trong mùa Tết, Trung Thu, Festival — tạo khan hiếm.
- Pet đồng hành: voi Tây Nguyên, mèo tam thể, chó phú quốc.

## **15.3. Vietnam Pass**

|**Tính năng**|**Free**|**Vietnam Pass**|
| :- | :- | :- |
|Số địa điểm truy cập|Tất cả|Tất cả + Early Access địa điểm mới|
|EXP Rate|x1|x1.5|
|Slot lưu ảnh game|100 ảnh|Không giới hạn|
|Tour tạo mỗi tháng|2 tour|Không giới hạn|
|Cosmetic mỗi tháng|Không|1 item ngẫu nhiên/tháng|
|Badge đặc biệt|Không|Badge "Vietnam Pass Member"|
|Hỗ trợ ưu tiên|Không|Có|

## **15.4. B2B — Quảng Bá Du Lịch Thật**

Đây là nguồn thu tiềm năng lớn nhất về dài hạn:

- Sở du lịch tỉnh/thành trả phí để địa điểm của tỉnh được xây dựng chi tiết và featured.
- Khách sạn, resort trả phí để nhân viên NPC giới thiệu cơ sở, hiển thị thông tin đặt phòng thật.
- Hãng hàng không tích hợp: người chơi mua vé máy bay trong game → redirect đến booking thật.
- Thương hiệu địa phương: cà phê Trung Nguyên, Vinamilk, v.v. tài trợ sự kiện hoặc NPC.

## **15.5. Nguyên Tắc Monetization**

- KHÔNG bán sức mạnh (Pay-to-Win) — không bán EXP tức thời, không mở khóa địa điểm bằng tiền thật.
- KHÔNG quảng cáo xâm lấn (intrusive ads) — không popup, không buộc xem quảng cáo.
- Minh bạch: tỷ lệ gacha (nếu có) phải hiển thị rõ ràng.
- Người chơi miễn phí phải có trải nghiệm đầy đủ và thú vị — trả tiền chỉ để cá nhân hóa nhiều hơn.


# **16. Pháp Lý & Dữ Liệu**


## **16.1. Bản Quyền & Sở Hữu Trí Tuệ**

|**Vấn đề**|**Mô tả**|**Giải pháp**|
| :- | :- | :- |
|Tên doanh nghiệp thật|Hiển thị tên quán ăn, khách sạn thật có thể cần xin phép|Xin phép hoặc dùng tên fictionalized gần đúng|
|Logo thương hiệu|Không được tái tạo logo có bản quyền|Dùng placeholder hoặc hợp tác chính thức|
|Ảnh thực địa|Ảnh chụp tòa nhà, cảnh quan cần kiểm tra bản quyền|Ưu tiên ảnh do người chơi upload (license CC)|
|Âm nhạc|Nhạc dân gian, nhạc cụ dân tộc trong game|Dùng nhạc CC0 hoặc tự sản xuất|
|Bản đồ OSM|OpenStreetMap dùng license ODbL|Phải ghi credit, không được bán lại dữ liệu OSM thuần|

## **16.2. Bảo Vệ Dữ Liệu Người Dùng**

- GDPR (EU): nếu có người chơi châu Âu → phải tuân thủ quyền xóa dữ liệu, quyền truy cập.
- Nghị định 13/2023/NĐ-CP (Việt Nam): bảo vệ dữ liệu cá nhân, cần có chính sách privacy rõ ràng.
- Dữ liệu người chơi (vị trí, hành vi, chat) không được bán cho bên thứ ba không liên quan.
- Dữ liệu hành vi dùng cho analytics phải được ẩn danh hóa.
- Người chơi dưới 13 tuổi: cần cơ chế riêng (COPPA nếu có người dùng Mỹ).

## **16.3. Giấy Phép & Đăng Ký**

|**Yêu cầu**|**Cơ quan**|**Ghi chú**|
| :- | :- | :- |
|Đăng ký trò chơi điện tử|Bộ TT&TT (Cục PTTH&TTĐT)|Bắt buộc trước khi phát hành tại VN|
|Phân loại độ tuổi|Bộ TT&TT|Cần gắn nhãn độ tuổi phù hợp|
|Giấy phép kinh doanh|Sở KH&ĐT|Nếu thu phí dịch vụ|
|Đăng ký bản quyền phần mềm|Cục Bản quyền tác giả|Bảo vệ sản phẩm|
|Hợp đồng với đối tác B2B|Pháp lý riêng từng trường hợp|Khách sạn, sở du lịch, hãng bay|

## **16.4. Nội Dung Nhạy Cảm**

- Lịch sử và chính trị: tránh thể hiện sai lệch sự kiện lịch sử Việt Nam.
- Ranh giới địa lý: bản đồ phải thể hiện đúng chủ quyền lãnh thổ Việt Nam.
- Tôn giáo: thể hiện tôn trọng các tín ngưỡng, tránh gây mâu thuẫn.
- Dân tộc thiểu số: thể hiện chính xác, tôn trọng văn hóa 54 dân tộc.
- Nội dung do cộng đồng tạo: cần hệ thống kiểm duyệt chặt tránh nội dung vi phạm pháp luật VN.

## **16.5. Bảo Mật Hệ Thống**

- Mã hóa toàn bộ dữ liệu người dùng: mật khẩu bcrypt, data-at-rest AES-256.
- HTTPS/TLS cho mọi giao tiếp client-server.
- Rate limiting: chống DDoS, brute force đăng nhập.
- Audit log: ghi lại mọi thao tác admin và thay đổi dữ liệu quan trọng.
- Penetration testing định kỳ trước mỗi lần ra mắt tính năng lớn.


# **17. Roadmap Kỹ Thuật Cực Chi Tiết**


## **17.1. Giai Đoạn 0 — Chuẩn Bị (Tháng 1-2)**

Mục tiêu: thiết lập môi trường phát triển và kiến trúc nền.

|**Task**|**Chi tiết**|**Thời gian**|
| :- | :- | :- |
|Cài đặt Unreal Engine 5|UE5.3+, cấu hình project, version control Git LFS|3 ngày|
|Học C++ cơ bản UE5|GameMode, PlayerController, Character, Actor, Component|2 tuần|
|Tích hợp Cesium for Unreal|Import plugin, cấu hình Cesium ion account, load tile Việt Nam|1 tuần|
|Kết nối OpenStreetMap|Dùng Overpass API để pull POI Hội An test|3 ngày|
|Thiết lập Supabase|Tạo project, schema ban đầu: users, characters, locations, sessions|2 ngày|
|Thiết kế database schema v1|ERD đầy đủ, quan hệ giữa bảng, index|3 ngày|
|CI/CD pipeline|GitHub Actions: build, test, deploy auto|3 ngày|

## **17.2. Giai Đoạn 1 — Single Player Prototype (Tháng 3-5)**

Mục tiêu: một người chơi đi bộ được trong bản đồ thật Hội An/Đà Nẵng.

|**Task**|**Chi tiết**|**Thời gian**|
| :- | :- | :- |
|Character controller|Đi bộ, chạy, nhảy, góc nhìn FPS/TPS, animation cơ bản|2 tuần|
|Cesium world map|Load terrain và building từ Cesium ion, Hội An khu vực 5km²|1 tuần|
|Spatial partitioning|Chia grid, load/unload chunk theo vị trí player|2 tuần|
|LOD system|Implement LOD 3 mức cho building, vegetation|1 tuần|
|POI system|Load địa điểm từ OSM, hiển thị marker, thông tin khi đến gần|1 tuần|
|Minimap|Render minimap realtime từ bản đồ OSM vector|1 tuần|
|Basic UI|HUD: vị trí, thời gian, compass, menu chính|1 tuần|
|Save/Load game|Lưu vị trí, trạng thái vào Supabase|3 ngày|

## **17.3. Giai Đoạn 2 — Multiplayer Core (Tháng 6-9)**

Mục tiêu: 2-50 người chơi gặp nhau trong cùng thế giới.

|**Task**|**Chi tiết**|**Thời gian**|
| :- | :- | :- |
|UE5 Dedicated Server|Cấu hình dedicated server build, deploy trên Oracle Cloud Free|2 tuần|
|Player replication|Đồng bộ vị trí, rotation, animation của tất cả người chơi|2 tuần|
|Epic Online Services|Đăng nhập, lobby, session management, friend list|2 tuần|
|Instance/Shard system|Tối đa 200 người/instance, auto-create instance khi đầy|2 tuần|
|Chat system|Text chat theo vùng (proximity), party chat, whisper|1 tuần|
|Anti-cheat cơ bản|Server authoritative movement, kiểm tra speed hack, teleport|1 tuần|
|Latency optimization|Dead reckoning, interpolation, lag compensation|2 tuần|
|Stress test|Load test 50 CCU, xác định bottleneck, optimize|1 tuần|

## **17.4. Giai Đoạn 3 — Gameplay Systems (Tháng 10-14)**

|**Hệ thống**|**Task chính**|**Thời gian**|
| :- | :- | :- |
|Phương tiện (xe máy, taxi)|Vehicle physics, thuê xe, trả xe, hệ thống đặt xe|4 tuần|
|NPC Layer 1 & 2|NPC lịch trình, pathfinding NavMesh, hội thoại cơ bản từ script|4 tuần|
|Quest system|Quest Manager, điều kiện trigger, tracking, reward|2 tuần|
|Inventory & Economy|Wallet tiền game, inventory vật phẩm, shop NPC|2 tuần|
|Lưu trú & Đặt phòng|Khách sạn system, check-in/out, hiển thị phòng|3 tuần|
|Ăn uống|Nhà hàng system, menu, gọi món, debuff/buff ăn uống|2 tuần|
|Thành tựu & Progression|Achievement system, EXP, level up, skill tree|2 tuần|
|Photo system|Chụp ảnh trong game, lưu vào gallery, share lên feed|1 tuần|

## **17.5. Giai Đoạn 4 — AI & Content (Tháng 15-20)**

|**Task**|**Chi tiết**|**Thời gian**|
| :- | :- | :- |
|Knowledge Graph xây dựng|Thu thập và cấu trúc dữ liệu Việt Nam vào graph database (Neo4j hoặc Supabase)|4 tuần|
|NPC Layer 3 — AI đầy đủ|Tích hợp LLM API, context injection, personality system|4 tuần|
|NPC Memory system|Short/episode/long-term memory, lưu vào Supabase|2 tuần|
|UGC Platform|Upload ảnh, submit địa điểm, tour builder, moderation queue|4 tuần|
|Recommendation engine|ML model gợi ý địa điểm, tour theo hành vi người chơi|3 tuần|
|Thời tiết thực|Tích hợp weather API, đồng bộ thời tiết theo location thật|1 tuần|
|Lễ hội & Sự kiện theo mùa|Event calendar, trigger nội dung theo ngày thực tế|2 tuần|

## **17.6. Giai Đoạn 5 — Scale & Polish (Tháng 21-24)**

|**Task**|**Chi tiết**|**Thời gian**|
| :- | :- | :- |
|Mở rộng bản đồ|Thêm 3-5 tỉnh/thành mới, chuẩn hóa quy trình onboard vùng mới|6 tuần|
|Mobile port (Android trước)|Unreal → Android build, optimize cho mobile hardware|8 tuần|
|Monetization systems|Shop, Vietnam Pass, payment gateway (MoMo, VNPay, Stripe)|3 tuần|
|Voice chat|Tích hợp Vivox hoặc Agora SDK, proximity voice|2 tuần|
|Performance audit|Profile toàn bộ game, giảm draw call, optimize shader, giảm memory|3 tuần|
|Localization|Tiếng Anh, tiếng Nhật, tiếng Hàn (theo thị trường mục tiêu)|2 tuần|
|Open Beta & Soft Launch|Mở cho 1.000 người chơi, thu thập feedback, fix bug critical|4 tuần|

## **17.7. Database Schema Chi Tiết**

|**Bảng (Table)**|**Cột chính**|**Quan hệ**|
| :- | :- | :- |
|users|id, email, password\_hash, created\_at, last\_login, status|1-1 với characters|
|characters|id, user\_id, name, level, exp, reputation\_json, position\_json, appearance\_json|1-N với inventories, sessions|
|locations|id, name, lat, lng, zone\_id, type, description, opening\_hours, rating|N-N với visits, reviews|
|visits|id, character\_id, location\_id, visited\_at, photo\_count|N-1 với characters & locations|
|quests|id, character\_id, quest\_type, status, progress\_json, started\_at, completed\_at|N-1 với characters|
|inventories|id, character\_id, item\_type, item\_id, quantity, equipped|N-1 với characters|
|npcs|id, name, location\_id, schedule\_json, personality\_json, memory\_json|N-1 với locations|
|sessions|id, server\_id, instance\_id, character\_ids\_json, zone\_id, created\_at|N-N với characters|
|ugc\_submissions|id, character\_id, type, data\_json, status, votes\_up, votes\_down, created\_at|N-1 với characters|
|tours|id, creator\_id, title, waypoints\_json, price, rating, participant\_count|N-1 với characters|
|transactions|id, character\_id, type, amount, currency, item\_ref, created\_at|N-1 với characters|

## **17.8. Stack Kỹ Thuật Tổng Hợp**

|**Layer**|**Công nghệ**|**Mục đích**|
| :- | :- | :- |
|Game Client|Unreal Engine 5 + C++|Render 3D, gameplay, UI, input|
|Multiplayer|UE5 Dedicated Server + EOS|Session, replication, matchmaking|
|Game Server backend|C++ / Go|Game logic, anti-cheat, world state|
|Database chính|PostgreSQL (Supabase)|Users, characters, quests, economy|
|Graph database|Neo4j hoặc Supabase + JSON|Knowledge Graph Việt Nam cho AI NPC|
|Cache|Redis|Session cache, leaderboard, hot data|
|File storage|Supabase Storage / S3|Ảnh UGC, asset game, backup|
|Map data|OSM + Cesium Ion + Overpass API|Bản đồ, địa điểm, terrain|
|AI NPC|LLM API (OpenAI / Anthropic / local model)|Hội thoại NPC Layer 3|
|Voice chat|Vivox hoặc Agora.io|Proximity voice, party chat|
|Analytics|PostHog hoặc Mixpanel|Hành vi người chơi, funnel, retention|
|Monitoring|Grafana + Prometheus|Server health, performance, alerts|
|CI/CD|GitHub Actions|Build, test, deploy tự động|
|CDN|Cloudflare|Asset delivery, DDoS protection|
|Payment|MoMo + VNPay + Stripe|Thanh toán Vietnam Pass, shop|

## **17.9. Checklist Trước Khi Ra Mắt**

Phase: Technical

- Load test: 500 CCU đồng thời không lag (< 100ms latency)
- Security audit: không có SQL injection, XSS, unauthorized API access
- Database: backup tự động, restore test thành công
- CDN: asset load time < 2s trên kết nối 4G Việt Nam
- Crash rate < 0.1% trong 48h stress test

Phase: Content

- Tối thiểu 1 thành phố hoàn chỉnh với 30+ POI
- 100+ NPC với lịch trình đầy đủ
- 20+ nhiệm vụ chính và 50+ nhiệm vụ phụ
- Tutorial onboarding dưới 5 phút
- Tất cả text tiếng Việt đã proofreading

Phase: Legal & Compliance

- Đã nộp hồ sơ đăng ký trò chơi điện tử lên Bộ TT&TT
- Privacy Policy và Terms of Service bằng tiếng Việt và Anh
- Nhãn phân loại độ tuổi hiển thị rõ
- Credit OSM và Cesium đúng quy định license


# **18. Art Direction & Visual Style (Định Hướng Nghệ Thuật)**


Vietnam Online theo đuổi phong cách Semi-Realistic (Bán thực) — đủ đẹp để người chơi nhận ra Việt Nam thật, đủ stylized để chạy được trên phần cứng phổ thông và không lỗi thời sau nhiều năm. Triết lý cốt lõi: Việt Nam phải nhìn ra Việt Nam, không phải một thành phố châu Âu hay Nhật Bản được đổi tên.

## **18.1. Triết Lý Nghệ Thuật Cốt Lõi**

|**Nguyên tắc**|**Mô tả**|**Ví dụ thực tế**|
| :- | :- | :- |
|Authenticity First|Mọi chi tiết phải có gốc rễ văn hóa Việt Nam thật|Màu ngói đỏ Hội An, không phải ngói xám Nhật Bản|
|Readable Silhouette|Nhân vật và công trình nhận ra được từ xa, trong mọi điều kiện ánh sáng|Nón lá luôn đọc được dù bóng tối hay ngược sáng|
|Atmosphere Over Accuracy|Cảm giác đúng quan trọng hơn đo đúng từng cm|Phố cổ Hà Nội phải có cảm giác chật, ẩm, xanh|
|Performance Conscious|Nghệ thuật đẹp nhưng phải chạy được 60fps trên GPU tầm trung|Texture atlas, LOD nghệ thuật, shader tối ưu|
|Timeless over Trendy|Tránh hiệu ứng thịnh hành nhất thời, chọn cái bền vững|Không dùng hyper-bloom, chromatic aberration quá đà|

## **18.2. Style Chính: Semi-Realistic**

Semi-Realistic là điểm giao giữa Realistic và Stylized — không cố photorealistic như The Last of Us, không cartoon như Genshin Impact. Tham chiếu style gần nhất:

|**Tham chiếu**|**Lấy gì từ game đó**|**KHÔNG lấy gì**|
| :- | :- | :- |
|Ghost of Tsushima|Ánh sáng tự nhiên đẹp, vegetation dày, bầu trời có hồn|Không lấy màu sắc Nhật Bản, particle quá nghệ|
|Red Dead Redemption 2|Ambient occlusion tinh tế, NPC sống thật, thời tiết có chiều sâu|Không lấy màu nâu vàng tây cổ điển|
|Microsoft Flight Simulator|Cách render mây, ánh sáng ban ngày, scale thế giới thật|Không lấy mức độ photoreal cần GPU cực mạnh|
|Stray|Cách stylize ánh đèn đường, ẩm ướt, texture cũ kỹ đô thị|Không lấy góc nhìn mèo và tỷ lệ kỳ lạ|
|Yakuza: Like a Dragon|Cách render đô thị châu Á đông đúc, biển quảng cáo, đèn neon|Không lấy màu sắc quá bão hòa của Nhật Bản|

## **18.3. Bảng Màu (Color Palette)**

Màu sắc được thiết kế theo vùng địa lý, thể hiện đặc trưng từng miền Việt Nam:

|**Vùng**|**Màu chủ đạo**|**Accent color**|**Cảm xúc cần truyền tải**|
| :- | :- | :- | :- |
|Hà Nội / Phố Cổ|Vàng ochre, xanh rêu, xám tường cũ|Đỏ đèn lồng, cam hoàng hôn|Lịch sử, chiều sâu, hoài cổ|
|Hội An|Vàng nghệ, trắng vôi, nâu gỗ tếch|Đỏ đèn lồng Hội An, xanh ngọc sông Thu Bồn|Thơ mộng, ấm áp, di sản|
|Đà Nẵng / Biển|Xanh biển đậm, trắng cát, xanh lam nhạt|Cam rực rỡ ban trưa, tím hoàng hôn|Tươi sáng, năng động, khoáng đạt|
|TP.HCM|Xám bê-tông, vàng kem, xanh lá nhiệt đới|Đỏ-vàng biển hiệu, trắng đèn neon|Sôi động, hiện đại, hỗn hợp|
|Tây Nguyên / Gia Lai|Đỏ đất bazan, xanh rừng thẫm, vàng cỏ tranh|Đỏ hoa dã quỳ, nâu đồng làng|Hoang dã, hùng tráng, nguyên sơ|
|Đà Lạt|Xanh thông, trắng sương mù, tím lavender|Hồng hoa dã quỳ, vàng mimosa|Lãng mạn, se lạnh, yên bình|
|Miền Tây sông nước|Xanh lá dừa, nâu sông, vàng lúa chín|Cam rực thuyền gỗ, trắng hoa sen|Thanh bình, trù phú, chậm rãi|

## **18.4. Ánh Sáng & Thời Tiết (Lighting & Weather)**

Ánh sáng là linh hồn của Semi-Realistic. Vietnam Online dùng Lumen (UE5) cho global illumination động:

|**Thời điểm**|**Đặc điểm ánh sáng**|**Màu sắc & cảm xúc**|
| :- | :- | :- |
|Bình minh (5h-7h)|Ánh sáng vàng ấm từ chân trời, sương nhẹ mặt hồ|Cam nhạt, vàng gold, tím pastel — yên tĩnh|
|Buổi sáng (7h-11h)|Ánh nắng trắng trong, bóng ngắn rõ nét|Trắng xanh trong trẻo — năng lượng|
|Ban trưa (11h-14h)|Nắng chói, bóng đen ngắn, haze nhiệt trên đường nhựa|Trắng khắc nghiệt, heat shimmer — ngột ngạt|
|Chiều tà (14h-17h)|Ánh vàng cam dài, bóng dài kéo, đẹp nhất ngày|Vàng amber, cam đào — thơ mộng nhất|
|Hoàng hôn (17h-19h)|Sky gradient tím-cam-hồng, silhouette rõ nét|Tím-cam rực rỡ — dramatic|
|Đêm thành phố (19h-24h)|Đèn đường vàng, neon màu sắc, ánh đèn cửa hàng|Warm artificial light — sôi động, ấm cúng|
|Đêm nông thôn / Tây Nguyên|Gần như không có ánh đèn, sao trời, đom đóm|Tím đen, xanh lạnh, chấm vàng — huyền bí|
|Mưa nhiệt đới|Ánh sáng xám lạnh, puddle reflection, mist|Xám xanh, bạc — ào ạt rồi tạnh ngay|
|Sương mù Đà Lạt buổi sáng|Volumetric fog dày, ánh sáng tán xạ diffuse|Trắng sữa mờ ảo — mơ màng|

**Kỹ thuật UE5 sử dụng:**

- Lumen Global Illumination: ánh sáng phản xạ gián tiếp thực tế, không cần bake lightmap.
- Lumen Reflections: phản chiếu mặt nước, kính, ướt sân.
- Volumetric Clouds & Fog: mây và sương mù 3D thực tế, thay đổi theo thời gian.
- Temporal Super Resolution (TSR): upscale chất lượng cao, tiết kiệm GPU.
- Nanite (chọn lọc): cho công trình kiến trúc chi tiết cao gần người chơi.

## **18.5. Kiến Trúc & Môi Trường**

Mỗi vùng có bộ asset riêng, không dùng lẫn lộn:

|**Loại công trình**|**Đặc điểm thiết kế**|**Chi tiết đặc trưng**|
| :- | :- | :- |
|Nhà phố Hà Nội|Mặt tiền hẹp, nhiều tầng, ban công sắt, cũ kỹ|Vữa bong tróc, dây điện chằng chịt, điều hòa chìa ra|
|Phố cổ Hội An|Tường vàng ochre, mái ngói âm dương, đèn lồng|Rêu ở góc tường, gỗ tối màu, hoa giấy leo|
|Biệt thự Đà Lạt|Phong cách Pháp, mái nhọn, cửa sổ vòm, vườn hoa|Đá xanh xây tường, mái ngói đỏ Pháp|
|Chung cư TP.HCM|Hiện đại, kính, bê-tông, bảng hiệu chi chít|Cây mọc ban công, quần áo phơi, máy lạnh|
|Nhà rông Tây Nguyên|Mái cao vút, tranh tre nứa, cột gỗ lớn|Hoa văn dân tộc, cồng chiêng treo, lửa giữa|
|Chùa / Đình|Mái cong lưỡng long chầu nguyệt, ngói âm dương|Tượng sơn son thếp vàng, hương khói, sân gạch|
|Chợ truyền thống|Mái tôn xô lệch, quầy hàng san sát, đèn huỳnh quang|Rau quả màu rực, thùng xốp, xe đẩy|

## **18.6. Nhân Vật (Character Art)**

Phong cách nhân vật bán thực — proportional gần với người thật nhưng có chút stylization ở khuôn mặt:

**Tỷ lệ cơ thể:**

- Đầu / Thân: 1:7 (người thật ~1:7.5) — đầu hơi lớn hơn chút để biểu cảm rõ hơn.
- Tay chân: tự nhiên, không phóng đại.
- Khuôn mặt: mắt hơi to hơn thực tế 10-15% để truyền cảm xúc tốt hơn.

**Đặc trưng nhân vật người Việt:**

- Tone da: dải từ sáng (người Kinh miền Bắc) đến nâu đậm (người dân tộc Tây Nguyên, ngư dân).
- Cấu trúc khuôn mặt: gò má nhẹ, mũi vừa, mắt hơi xếch — đặc trưng Đông Nam Á.
- Tóc: đen bóng là default, đuôi ngựa, tóc ngắn, tóc xõa theo vùng và nghề nghiệp.
- Trang phục NPC thay đổi theo nghề: áo bà ba ngư dân, đồng phục học sinh, vest doanh nhân.

**Avatar người chơi:**

- Character creator: chọn giới tính, tone da (12 mức), kiểu mặt (8 preset + sliders), tóc (30+ kiểu).
- Quốc tịch người chơi có thể chọn, ảnh hưởng NPC phản ứng (người nước ngoài NPC chào bằng tiếng Anh).
- Trang phục thay đổi thấy trên toàn bộ người chơi khác — không invisible cosmetic.

## **18.7. Thực Vật & Thiên Nhiên**

|**Loại thực vật**|**Đặc điểm kỹ thuật**|**Phân bố**|
| :- | :- | :- |
|Cây đa / Cây si|Rễ buông dài, tán rộng, lá dày|Sân đình, cổng làng, ven đường cũ|
|Dừa nước / Dừa|Thân cong, lá dài đong đưa theo gió|Miền Tây, ven biển, Hội An|
|Thông 3 lá|Thân thẳng, tán tháp, lá kim dày|Đà Lạt — đặc trưng nhất|
|Rừng khộp Tây Nguyên|Thưa, thân cong, lá vàng mùa khô|Gia Lai, Đắk Lắk|
|Lúa & ruộng bậc thang|Shader gió làm sóng lúa, màu xanh non đến vàng theo mùa|Sapa, miền Bắc, đồng bằng|
|Sen, súng, bèo|Shader nước, lá nổi, hoa nở theo giờ|Hồ, ao làng, sông Hương|
|Hoa dã quỳ|Màu vàng rực, mọc thành đám lớn|Đà Lạt, Gia Lai mùa cuối năm|

**Kỹ thuật vegetation:**

- Wind shader: cây, cỏ, lúa đung đưa theo hướng gió động.
- Season system: màu sắc lá cây thay đổi theo mùa (xanh non / xanh đậm / vàng / rụng lá).
- Procedural scatter: cỏ, hoa, đá nhỏ rải tự động theo terrain type.
- LOD vegetation: cây gần đầy đủ geometry, cây xa là billboard 2D.

## **18.8. Hiệu Ứng Đặc Biệt (VFX)**

|**Hiệu ứng**|**Mô tả kỹ thuật**|**Khi nào xuất hiện**|
| :- | :- | :- |
|Mưa nhiệt đới|Particle rain dày, splash puddle, rivulet trên mái|Thời tiết mưa, đặc biệt mùa hè|
|Khói bếp than|Volumetric smoke nhẹ, màu xanh xám|Quán ăn vỉa hè, làng chài|
|Hương nhang / Khói chùa|Particle khói trắng uốn lượn, ánh sáng tán xạ|Chùa, đền, nhà có bàn thờ|
|Đom đóm|Particle phát sáng xanh lá, bay chậm random|Đêm vùng nông thôn, Tây Nguyên|
|Đèn lồng bay (Hội An)|Physics-based lantern float, ánh sáng ấm|Đêm rằm, lễ hội Hội An|
|Sóng biển & bọt sóng|Ocean shader + foam particle|Bãi biển, thuyền trên sông|
|Bụi đường đất|Particle bụi theo chân người, xe|Đường đất Tây Nguyên, mùa khô|
|Pháo hoa Tết|Particle burst nhiều màu, trail sáng|Sự kiện Tết Nguyên Đán|
|Heat haze|Screen-space distortion nhẹ|Đường nhựa trưa hè, sa mạc cát|
|Mist sông buổi sáng|Volumetric fog thấp trên mặt nước|Sông Hương, Mekong, Hồ Tây|

## **18.9. UI / UX Visual Design**

Giao diện người dùng phải đồng nhất với thế giới Semi-Realistic nhưng không cản trở gameplay:

|**Element UI**|**Style**|**Ghi chú**|
| :- | :- | :- |
|HUD chính|Minimal, trong suốt, góc màn hình|Ẩn hoàn toàn khi chụp ảnh (Photo Mode)|
|Font chính|Noto Sans (hỗ trợ tiếng Việt dấu đầy đủ), sans-serif sạch|Fallback: SVN-Gilroy|
|Font tiêu đề|Serif cổ điển gợi bản đồ cổ, có nét mực|Dùng cho tên địa điểm, màn hình loading|
|Minimap|Phong cách bản đồ giấy hand-drawn, không digital neon|Tông màu nâu vàng, nét mực|
|Menu chính|Ảnh nền ambient cảnh Việt Nam đang động (video)|Đổi ảnh theo mùa/sự kiện|
|Inventory|Túi da / Ba lô kết cấu, item hiển thị 3D|Drag & drop, tooltip mô tả item|
|Quest log|Sổ tay tay cầm tay, chữ viết tay style|Có thể vẽ nguệch ngoạc trên bản đồ|
|Notification|Toast popup mỏng, bên cạnh, tự biến mất|Không dùng popup giữa màn hình cắt gameplay|
|Dialog NPC|Khung thoại phía dưới, ảnh avatar NPC bên trái|Không dùng text box nổi giữa màn hình|

## **18.10. Âm Thanh & Âm Nhạc (Audio Identity)**

Âm thanh là 50% trải nghiệm. Vietnam Online có hệ thống âm thanh không gian 3D (spatial audio):

**Ambient soundscape theo vùng:**

|**Địa điểm**|**Ambient sound chính**|**Chi tiết**|
| :- | :- | :- |
|Phố cổ Hà Nội sáng sớm|Tiếng rao hàng rong, xe đạp, chim hót|"Bánh mì nóng! Xôi nóng đây!"|
|Chợ buổi sáng|Ồn ào mặc cả, tiếng thái rau, loa thông báo|Thực tế ghi âm chợ Đồng Xuân|
|Biển Đà Nẵng buổi chiều|Sóng biển, gió, tiếng cười trẻ em|Layered: gần mạnh, xa nhẹ|
|Tây Nguyên ban đêm|Cồng chiêng từ xa, côn trùng, tiếng gió rừng|Reverb dài, không gian mở|
|Mưa nhiệt đới|Mưa trên mái tôn, mái ngói, mái tranh — 3 loại khác nhau|Dynamic theo loại mái bên trên|
|Đền chùa|Tiếng chuông, mõ, kinh tụng nhẹ xa xa|Reverb phòng lớn, ambience thánh|
|TP.HCM ban đêm|Kẹt xe, còi xe, nhạc từ quán bar, tiếng mưa|Dense layer, energy cao|

**Âm nhạc nền (OST):**

- Nhạc cụ dân tộc làm nền: đàn tranh, đàn bầu, sáo trúc, cồng chiêng Tây Nguyên.
- Pha trộn với electronic ambient nhẹ — không quá truyền thống, không quá hiện đại.
- Adaptive music: nhạc thay đổi cường độ theo tốc độ di chuyển và tình huống.
- Mỗi vùng có theme âm nhạc riêng nhận ra được sau 5 giây nghe.
- Sự kiện đặc biệt có nhạc riêng: nhạc Tết, nhạc Festival Hội An, nhạc buổi sáng sớm.

## **18.11. Photo Mode**

Photo Mode là tính năng quan trọng — người chơi sẽ chia sẻ ảnh lên mạng xã hội, tạo marketing tự nhiên:

- Ẩn hoàn toàn HUD, tự do di chuyển camera trong bán kính 20m quanh nhân vật.
- Điều chỉnh: FOV, depth of field, exposure, contrast, vignette, film grain.
- Filter gợi ý theo vùng: "Hội An Golden", "Tây Nguyên Rust", "Đà Lạt Misty", "Saigon Night".
- Pose library: 30+ tư thế nhân vật, có thể mix với emote.
- Sticker văn hóa: nón lá, đèn lồng, hoa sen — overlay trang trí ảnh.
- Ảnh xuất ra watermark logo Vietnam Online góc nhỏ (có thể tắt cho Premium).
- Ảnh được lưu vào Gallery trong game, có thể đăng lên feed cộng đồng.

## **18.12. Pipeline Nghệ Thuật**

Quy trình sản xuất asset từ concept đến in-game:

|**Bước**|**Công việc**|**Tool**|**Output**|
| :- | :- | :- | :- |
|1\. Concept Art|Sketch style direction, color key, mood board từng vùng|Procreate / Photoshop|Concept sheet A3|
|2\. Blockout|Dựng hình khối 3D thô, kiểm tra scale và proportion|Blender / UE5|Mesh blockout|
|3\. High-poly Model|Model chi tiết cao, capture normal map|Blender / ZBrush|High-poly mesh|
|4\. Low-poly & Retopo|Retopology thân thiện game, UV unwrap|Blender / Maya|Game-ready mesh|
|5\. Texture|PBR texturing: Albedo, Normal, Roughness, Metallic|Substance Painter|4K texture set|
|6\. LOD Generation|Tạo 3-4 mức LOD tự động + kiểm tra thủ công|UE5 LOD tool|LOD0-LOD3|
|7\. In-engine Setup|Import UE5, cài material, lighting test, collision|Unreal Engine 5|Placed asset|
|8\. QA & Optimization|Kiểm tra performance, draw call, visual consistency|UE5 Profiler|Approved asset|

**Tiêu chuẩn kỹ thuật asset:**

- Building nhỏ (nhà phố): tối đa 5.000 triangle LOD0, 500 LOD3.
- Building lớn (chùa, khách sạn): tối đa 20.000 triangle LOD0.
- NPC: 15.000-25.000 triangle (có rigged skeleton), texture 2K.
- Xe máy: 8.000 triangle, texture 2K PBR.
- Cây lớn: 3.000 triangle + billboard LOD, wind shader.
- Texture atlas: nhóm các prop nhỏ vào 1 atlas 4K để giảm draw call.


*— Hết tài liệu — Vietnam Online Project Documentation v1.2 —*
Trang 
