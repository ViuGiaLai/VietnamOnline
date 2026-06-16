Vietnam Online  ·  Atomic / Component / Detail Level
|<p>**🇻🇳  VIETNAM ONLINE**</p><p>**ATOMIC / COMPONENT / DETAIL LEVEL**</p><p>*Tầng Vi Mô Cực Nhỏ — Game Design Specification*</p><p>v1.0  ·  Dựa trên 4 tài liệu thiết kế Vietnam Online</p>|
| :-: |


# **📋  MỤC LỤC TỔNG QUÁT**

|**#**|**Module Atomic**|**Số Atom**|**Trang**|
| :-: | :-: | :-: | :-: |
|A-01|PlayerEntity Atom|12 atoms|→ §A|
|A-02|InputAction Atom|18 atoms|→ §B|
|A-03|StaminaSystem Atom|8 atoms|→ §C|
|A-04|VehiclePhysics Atom|14 atoms|→ §D|
|A-05|FoodBuff Atom|10 atoms|→ §E|
|A-06|POI Atom|11 atoms|→ §F|
|A-07|Currency Atom|9 atoms|→ §G|
|A-08|NPC\_AI Atom|15 atoms|→ §H|
|A-09|ChatMessage Atom|7 atoms|→ §I|
|A-10|Quest Atom|13 atoms|→ §J|
|A-11|PhotoScore Atom|8 atoms|→ §K|
|A-12|WeatherState Atom|9 atoms|→ §L|
|A-13|Reputation Atom|6 atoms|→ §M|
|A-14|ServerSync Atom|11 atoms|→ §N|
|A-15|AudioState Atom|8 atoms|→ §O|
|A-16|UIComponent Atom|16 atoms|→ §P|
|A-17|OnboardingEvent Atom|7 atoms|→ §Q|
|A-18|RetentionTrigger Atom|9 atoms|→ §R|



|**§A — PlayerEntity Atom**  |  Tầng Nguyên Tử: Nhân Vật Người Chơi|
| :- |

## **A.1  Định Nghĩa Struct PlayerEntity**
*ℹ️  GHI CHÚ KỸ THUẬT: Đây là unit nhỏ nhất không thể phân tách — mọi system khác đọc/ghi vào struct này.*

|**Field**|**Kiểu Dữ Liệu**|**Giá Trị Mặc Định**|**Ràng Buộc / Validation**|
| :-: | :-: | :-: | :-: |
|player\_id|UUID v4 (string)|Auto-gen khi register|Immutable sau khi tạo. Unique trên toàn DB.|
|display\_name|string (32 chars)|""|Min 2 ký tự, Max 32. Không chứa ký tự đặc biệt ngoài \_-.|
|pos\_x|float64|0\.0|Đơn vị: mét Unreal. Valid nếu nằm trong NavMesh.|
|pos\_y|float64|0\.0|Kiểm tra boundary mỗi server tick (50ms).|
|pos\_z|float64|0\.0|Giá trị âm = dưới mặt đất → server clamp về 0.|
|zone\_id|string (8 chars)|"HOI\_AN"|Phải tồn tại trong ZoneRegistry. Đổi zone = loading trigger.|
|stamina\_cur|float32|100\.0|Range [0.0, stamina\_max]. Server-authoritative.|
|stamina\_max|float32|100\.0|Base 100. Có thể tăng bởi buff. Max hard cap: 150.|
|vnd\_balance|int64|0|Không âm. Max 500,000,000. Trừ tiền phải atomic transaction.|
|xu\_vang|int32|0|Không âm. Không có hard cap. Hết hạn sau 730 ngày.|
|explorer\_lv|int16|1|Range [1, 100]. Không giảm dù reset.|
|cultural\_knowledge|int16|0|Range [0, 1000]. Điểm tích lũy tri thức văn hóa.|
|reputation\_score|float32|3\.0|Range [0.0, 5.0]. Trung bình có trọng số 50 review gần nhất.|
|active\_vehicle|nullable VehicleRef|null|null = đi bộ. Non-null = đang trên phương tiện.|
|active\_buffs|[]BuffEntry|[]|Max 5 buff cùng lúc. FIFO khi vượt.|
|photo\_score\_elo|int32|1000|Range [500, ∞]. Tăng khi ảnh được like/mua.|

## **A.2  State Machine Trạng Thái Nhân Vật**
*⚠️  CẢNH BÁO: Nhân vật chỉ có thể ở đúng 1 state tại 1 thời điểm. State transition được validate server-side.*

|**State**|**Mô Tả**|**Transition VÀO**|**Transition RA**|**Đặc Điểm**|
| :-: | :-: | :-: | :-: | :-: |
|IDLE|Đứng yên, không hành động|Từ WALKING khi tốc độ = 0|Input di chuyển|Stamina hồi +2/s|
|WALKING|Di chuyển tốc độ 1x (~5km/h)|Input WASD nhẹ|Sprint input / dừng|Stamina không hao|
|SPRINTING|Tốc độ x1.8 (~9km/h)|Shift + WASD|Stamina=0 / thả Shift|Stamina -1/s|
|RIDING\_VEHICLE|Đang điều khiển phương tiện|Nhấn E ở trạm xe|Trả xe / hết xăng|Physic khác hẳn đi bộ|
|PASSENGER|Ngồi taxi/xe khách, không điều khiển|Đặt taxi confirm|Đến điểm / hủy chuyến|Không tương tác world|
|INTERACTING|Đang dialogue với NPC|Click NPC trong 3m|Thoát dialogue / hết câu|Không thể di chuyển|
|PHOTO\_MODE|Camera mode, không di chuyển|Phím F8 / nút camera|Phím Esc / chụp xong|HUD ẩn hoàn toàn|
|SITTING\_REST|Ngồi nghỉ tại ghế/quán|Tương tác với điểm ngồi|Input di chuyển / đứng|Stamina +5/s|
|EATING|Animation ăn (3 giây)|Chọn món và nhận|Animation xong tự động|Nhận buff ngay khi kết thúc|

## **A.3  Movement Speed Matrix**

|**State**|**Speed (km/h game)**|**Speed (m/s code)**|**Modifier Conditions**|
| :-: | :-: | :-: | :-: |
|WALKING bình thường|5\.0|1\.39|Baseline — không modifier|
|WALKING trời nắng gắt|4\.0|1\.11|12h-14h, mùa hè, ngoài trời → -20%|
|WALKING stamina = 0|2\.5|0\.69|Stamina hết → -50%, không sprint được|
|SPRINTING|9\.0|2\.50|Shift held + stamina > 0|
|SPRINTING buff Phở Bò|9\.9|2\.75|+10% tốc độ đi bộ từ buff (2h)|
|RIDING Honda Wave (hẻm)|60\.0|16\.67|Xe số, max speed trong hẻm nhỏ|
|RIDING Yamaha Exciter|80\.0|22\.22|Không vào hẻm nhỏ (width check)|
|RIDING Honda SH|70\.0|19\.44|Turn radius rộng nhất|
|RIDING Xe đạp|25\.0|6\.94|Lên dốc -40%, xuống dốc +20%|



|**§B — InputAction Atom**  |  Tầng Nguyên Tử: Hệ Thống Input|
| :- |

## **B.1  Input Binding Table (PC / Gamepad / Mobile)**

|**Action ID**|**PC (KB/Mouse)**|**Gamepad**|**Mobile**|**Priority**|**Cooldown**|
| :-: | :-: | :-: | :-: | :-: | :-: |
|ACT\_MOVE\_FORWARD|W / Arrow Up|Left Stick Up|Virtual Joystick Up|REALTIME|0ms|
|ACT\_MOVE\_BACK|S / Arrow Down|Left Stick Down|Virtual Joystick Down|REALTIME|0ms|
|ACT\_MOVE\_LEFT|A / Arrow Left|Left Stick Left|Virtual Joystick Left|REALTIME|0ms|
|ACT\_MOVE\_RIGHT|D / Arrow Right|Left Stick Right|Virtual Joystick Right|REALTIME|0ms|
|ACT\_SPRINT|Shift (hold)|R2 hold|Sprint Button (hold)|REALTIME|0ms|
|ACT\_INTERACT|E|A / Cross|Tap Interact Button|PRESSED|200ms|
|ACT\_MAP\_OPEN|M|Select/Options|Tap Map Icon|PRESSED|300ms|
|ACT\_PHOTO\_MODE|F8|L1+R1 simultaneous|Camera Icon HUD|PRESSED|500ms|
|ACT\_CHAT\_OPEN|Enter|D-Pad Down|Chat Icon|PRESSED|0ms|
|ACT\_PARTY\_INVITE|Ctrl+I|D-Pad Up|Long-press Player Name|PRESSED|1000ms|
|ACT\_QUEST\_LOG|J|D-Pad Left|Quest Icon|PRESSED|300ms|
|ACT\_INVENTORY|I|D-Pad Right|Bag Icon|PRESSED|300ms|
|ACT\_CAMERA\_LOOK|Mouse Move|Right Stick|Swipe Right Area|REALTIME|0ms|
|ACT\_ZOOM\_IN|Scroll Up|—|Pinch In|REALTIME|0ms|
|ACT\_EMOTE|Ctrl+E menu|Y / Triangle|Emote Icon|PRESSED|500ms|
|ACT\_VOICE\_PTT|V (hold)|L1 hold|Mic Button hold|REALTIME|0ms|
|ACT\_CANCEL / BACK|Esc / RMB|B / Circle|Back Arrow|PRESSED|0ms|
|ACT\_CONFIRM|Enter / Space|A / Cross|Tap Confirm|PRESSED|150ms|

## **B.2  Input Validation Rules**
*🚫  CRITICAL: Mọi input phải qua validation layer trước khi gửi lên server. Client chỉ 'dự đoán' — server xác nhận.*

|**Rule ID**|**Điều Kiện**|**Kết Quả Nếu Vi Phạm**|**Xử Lý**|
| :-: | :-: | :-: | :-: |
|IV-01|Tốc độ thay đổi vị trí > max\_speed \* 1.5 trong 1 tick|Nghi ngờ speed hack|Server từ chối, rollback vị trí, ghi log|
|IV-02|Vị trí teleport > 500m/giây|Nghi ngờ teleport hack|Rollback + flag tài khoản + điều tra|
|IV-03|Input frequency > 120 actions/giây|Bot hoặc macro|Rate limit, sau 3 lần: CAPTCHA|
|IV-04|Interact trong khi SPRINTING|Không hợp lệ|Ignore input, không response|
|IV-05|PHOTO\_MODE trong khi RIDING\_VEHICLE|Không hợp lệ|Auto-exit vehicle trước, rồi enter photo|
|IV-06|Di chuyển vào tọa độ ngoài NavMesh|Out of bounds|Clamp về edge NavMesh gần nhất|
|IV-07|Input từ multiple device cùng lúc|Sesion hijack nghi ngờ|Giữ session device login trước, log out device khác|
|IV-08|Phản hồi input < 100ms đều đặn (bot pattern)|Bot farming|CAPTCHA challenge sau 5 phút|



|**§C — StaminaSystem Atom**  |  Tầng Nguyên Tử: Hệ Thống Thể Lực|
| :- |

## **C.1  Stamina Delta Table — Mọi Condition**

|**Condition ID**|**Trạng Thái / Trigger**|**Delta /giây**|**Áp Dụng Khi**|
| :-: | :-: | :-: | :-: |
|STM-01|Đi bộ bình thường (WALKING)|+0.0|Luôn khi WALKING, không modifier|
|STM-02|Chạy bộ (SPRINTING)|−1.0|Shift held + stamina > 0|
|STM-03|Đứng yên (IDLE)|+2.0|Không phải EATING/INTERACTING|
|STM-04|Ngồi nghỉ (SITTING\_REST)|+5.0|Chỉ khi SITTING\_REST state|
|STM-05|Ngủ trong khách sạn/hostel|+15.0|Nhân vật 'ngủ' (skip time mode)|
|STM-06|Nắng gắt 12h-14h ngoài trời|−0.5|Thêm vào delta hiện tại, mọi state trừ trong nhà|
|STM-07|Đói (>8h game chưa ăn)|−0.3 max penalty|max\_stamina bị giảm 30%, không phải delta tức thì|
|STM-08|Buff Phở Bò active|Nhân delta hồi x1.25|2h sau khi ăn phở bò|
|STM-09|Buff Cháo Lòng active|Nhân delta hồi x2.0|2h sau khi ăn cháo lòng|
|STM-10|Vào bóng mát (nhiệt độ cao)|+1.5 bù trừ nắng|Thay thế STM-06 khi vào vùng bóng mát|
|STM-11|Resort Spa buff (4h)|max\_stamina +20 flat|Sau khi dùng spa resort 5⭐|

## **C.2  Stamina Threshold Effects**

|**Range Stamina**|**Visual Cue**|**Gameplay Effect**|**Audio**|
| :-: | :-: | :-: | :-: |
|80-100%|Bar màu xanh lá|Không ảnh hưởng|Không|
|50-79%|Bar màu xanh nhạt|Không ảnh hưởng|Không|
|20-49%|Bar màu vàng cam|Không ảnh hưởng|Không|
|1-19%|Bar màu đỏ, nhấp nháy|Sprint bị tắt tự động|Tiếng tim đập nhẹ|
|0%|Bar trống, icon mệt|Speed −50%, sprint không được|Thở dốc liên tục|

*✅  BEST PRACTICE: Stamina = 0 KHÔNG gây chết nhân vật. Chỉ làm chậm 50% tốc độ. Vietnam Online không có punishing mechanics.*



|**§D — VehiclePhysics Atom**  |  Tầng Nguyên Tử: Vật Lý Phương Tiện|
| :- |

## **D.1  Vehicle Config Parameters (UE5 Vehicle Plugin)**

|**Param**|**Honda Wave (Số)**|**Yamaha Exciter**|**Honda SH (Tay Ga)**|**Xe Đạp**|**Xích Lô**|
| :-: | :-: | :-: | :-: | :-: | :-: |
|max\_speed\_kmh|60|80|70|25|15|
|mass\_kg|102|127|130|12|80|
|friction\_coeff|0\.85|0\.78|0\.80|0\.90|0\.88|
|turn\_radius\_m|3\.2|4\.5|5\.0|2\.0|6\.0|
|suspension\_spring|35\.0|42\.0|45\.0|15\.0|20\.0|
|engine\_torque|12\.5 Nm|18\.0 Nm|15\.0 Nm|N/A|N/A|
|gear\_count|4 (manual shift)|Auto (CVT)|Auto (CVT)|N/A|N/A|
|fuel\_tank\_km|150|200|180|∞ (không xăng)|∞|
|alley\_access|✅ (width ≥ 1.2m)|❌ (> 1.6m)|❌ (> 1.8m)|✅ (≥ 0.8m)|✅|
|rental\_cost\_hour|20,000đ|35,000đ|45,000đ|10,000đ|30,000đ (NPC fixed)|
|unlock\_phase|Phase 1 (MVP)|Phase 2|Phase 2|Phase 1 (MVP)|Phase 2|

## **D.2  Rental Flow — State Machine (< 5 giây target)**
*🚫  CRITICAL: Toàn bộ rental flow phải hoàn tất trong < 5 giây, không loading screen, không scene change.*

|**Step**|**Trigger**|**Action**|**Duration**|**Thất Bại Condition**|
| :-: | :-: | :-: | :-: | :-: |
|Step 1: Proximity|Player vào radius 10m của trạm|Icon xe nhấp nháy góc màn hình|0ms (realtime)|Không có xe khả dụng → icon xám + tooltip|
|Step 2: Open Panel|Nhấn [E] / tap icon|Panel slide-up từ dưới (danh sách xe, giá, số còn)|300ms animation|Balance < giá thuê → nút Thuê disabled + tooltip|
|Step 3: Select & Confirm|Chọn xe + tap Confirm|1 click confirm — không cần bước 2|150ms response|Server validation fail → show error inline|
|Step 4: Spawn Vehicle|Server confirm OK|Animation nhân vật leo lên xe (1.5s)|1500ms|Vị trí spawn bị block → thử spawn offset 2m|
|Step 5: HUD Update|Animation xong|HUD: icon xe + đồng hồ tính giờ + nút Trả Xe|100ms|Không thể fail ở bước này|

## **D.3  Fuel System Atom**

|**Event**|**Fuel Delta**|**Trigger Condition**|**Player Feedback**|
| :-: | :-: | :-: | :-: |
|Di chuyển bình thường|−1 đơn vị / km game|Xe đang chạy, không idle|Fuel gauge trên HUD|
|Idle (đứng máy)|−0.1 đơn vị / phút|Engine on, vận tốc = 0|Nhẹ hơn, gần như không đáng kể|
|Đổ xăng tại trạm|+đầy bình|Player ở trong 5m trạm xăng, nhấn E|Animation đổ xăng (2s), trừ 15,000đ|
|Hết xăng (fuel = 0)|Xe đứng máy|Bất kỳ khi nào fuel = 0 trong khi chạy|Engine sputter sound, xe dừng dần|
|Cảnh báo sắp hết|Không|Fuel ≤ 20% bình|Icon xăng nhấp nháy đỏ + beep nhẹ|
|Waypoint gợi ý|Không|Fuel ≤ 10% bình|Minimap tự thêm waypoint trạm xăng gần nhất|



|**§E — FoodBuff Atom**  |  Tầng Nguyên Tử: Buff Ăn Uống|
| :- |

## **E.1  BuffEntry Data Structure**

|**Field**|**Type**|**Mô Tả**|
| :-: | :-: | :-: |
|buff\_id|string|UUID tự sinh mỗi lần ăn. Dùng để track và xóa buff.|
|buff\_type|enum BuffType|STAMINA\_REGEN | SPEED\_BOOST | EXP\_BONUS | PRICE\_DISCOUNT | PHOTO\_BONUS|
|source\_food|string|Tên món ăn tạo ra buff. Chỉ để hiển thị UI.|
|value\_primary|float32|Giá trị chính. Ví dụ: 0.25 = +25% stamina hồi.|
|value\_secondary|float32 | null|Giá trị phụ (nếu có). Phở Bò: +0.10 cho speed.|
|duration\_seconds|int32|Thời gian buff tính bằng giây game-time (1:1 với thực).|
|remaining\_seconds|int32|Countdown realtime. Hết = xóa buff khỏi active\_buffs.|
|stack\_behavior|enum|REFRESH (ăn thêm reset timer) | NO\_STACK (không ăn chồng)|
|region\_lock|[]string | null|null = toàn quốc. ['HUE'] = chỉ có ở Huế.|
|applied\_at|timestamp|Thời điểm buff được áp dụng. Server time.|

## **E.2  Buff Registry — Toàn Bộ Đặc Sản**

|**Món Ăn**|**Region**|**Buff Type**|**Value**|**Duration**|**Giá (đ game)**|**Special Condition**|
| :-: | :-: | :-: | :-: | :-: | :-: | :-: |
|Phở Bò|HN|STAMINA\_REGEN + SPEED|+25% regen / +10% walk|2h|35k-60k|Chỉ hiệu quả ban sáng 6h-10h (+15% thêm)|
|Bún Bò Huế|HUE|SPEED + ANTI\_NAUSEA|+15% xe máy, chống say|3h|40k-70k|Anti\_nausea: không debuff khi lên dốc|
|Cao Lầu|HOI\_AN|EXP\_BONUS(culture)|+30% Cultural Know gain|2h|40k-65k|Chỉ mua được tại Hội An. Không ship đi zone khác.|
|Bánh Mì|NATIONWIDE|STAMINA\_SMALL + SPEED|+10% nhỏ, ăn di chuyển|1h|15k-30k|Có thể ăn trong khi WALKING/RIDING|
|Cơm Lam|GIA\_LAI|EXP\_BONUS(explorer)|+40% Explorer EXP trong rừng|4h|50k-80k|Chỉ hoạt động khi ở zone rừng núi|
|Gỏi Cuốn|MIEN\_NAM|PRICE\_DISCOUNT|−10% giá NPC 2h|2h|25k-45k|Discount áp dụng mọi dịch vụ NPC|
|Cháo Lòng|NATIONWIDE|STAMINA\_REGEN|×2 stamina khi rest|2h|20k-40k|Stack với SITTING\_REST (+5/s → +10/s)|
|Cà Phê Trứng|HN|PHOTO\_BONUS|+50% Photo Score 5 ảnh tới|1h|30k-50k|Đếm số ảnh, không phải thời gian|
|Cà Phê Sữa Đá|HCMC|SPEED|+10% mọi phương tiện|1\.5h|20k-35k|Không stack với Bún Bò Huế|
|Bánh Xèo|MIEN\_TRUNG|SOCIAL\_BONUS|+20% Social EXP|2h|35k-55k|Chỉ unlock sau Cultural Knowledge ≥ 50|

## **E.3  Order Flow — Chi Tiết Từng Bước**

|**Step**|**Trigger**|**Server Action**|**Client Feedback**|**Time**|
| :-: | :-: | :-: | :-: | :-: |
|1\. Approach table|Player trong 3m bàn|None (pure client)|Highlight table, interact icon|0ms|
|2\. NPC approaches|Player giữ proximity 1s|NPC path đến table|NPC animation đi đến|1-2s|
|3\. Menu open|NPC đến hoặc manual tap|Fetch menu từ DB nhà hàng|Menu modal: ảnh, mô tả, giá, buff preview|< 200ms|
|4\. Item select|Tap món|None|Highlight món, show buff tooltip|0ms|
|5\. Order confirm|Tap Đặt Món|Validate balance, deduct 20% deposit|Cook timer start (30-90s tùy món)|< 100ms|
|6\. Food arrives|Timer end|Apply buff to player\_buffs|Animation bưng món, notification|0ms|
|7\. Eat animation|Auto trigger|None (cosmetic)|3 giây eat animation|3000ms|
|8\. Buff applied|Animation done|BuffEntry insert vào active\_buffs|HUD buff icon appear + timer|50ms|
|9\. Payment|Player leave table / manual|Deduct remaining balance|Receipt notification|< 100ms|



|**§F — POI Atom**  |  Tầng Nguyên Tử: Điểm Tham Quan|
| :- |

## **F.1  POI Data Schema**

|**Field**|**Type**|**Tier Áp Dụng**|**Validation**|
| :-: | :-: | :-: | :-: |
|poi\_id|UUID|Tất cả|Auto-gen. Immutable.|
|name\_vi|string 200|Tất cả|Required. Tên chính thức tiếng Việt.|
|name\_en|string 200|T1, T2|Optional T3/T4. Required T1/T2.|
|lat|float64 (6 decimal)|Tất cả|Valid GPS coord Việt Nam. Từ OSM.|
|lng|float64 (6 decimal)|Tất cả|Valid GPS coord Việt Nam. Từ OSM.|
|tier|enum [1,2,3,4]|Tất cả|Immutable sau khi set. Upgrade cần manual review.|
|category|enum|Tất cả|HISTORY | NATURE | SPIRITUAL | FOOD | ENTERTAINMENT | HERITAGE|
|check\_in\_radius\_m|int16|Tất cả|T1: 100m, T2: 75m, T3: 50m, T4: 30m|
|requires\_photo|bool|T1 bắt buộc|true = phải chụp ảnh xác nhận mới checkin|
|open\_hours|JSON|T1, T2|{ mon: '07:00-17:00', ... }. null = mở 24/7.|
|entry\_fee\_vnd|int32 | null|T1, T2|null = miễn phí. Số = giá tiền game.|
|description\_vi|text|T1, T2|Min 500 từ T2. Min 2000 từ T1.|
|audio\_guide\_url|string | null|T1 only|URL file audio hướng dẫn. T2+ null.|
|exp\_reward|int32|Tất cả|T1: 500-1000. T2: 100-500. T3: 50. T4: 10.|
|first\_discovery\_bonus|int32|Tất cả|exp\_reward × 5 cho người checkin đầu tiên.|
|is\_hidden|bool|20% POI|true = không hiện map mặc định. Unlock via NPC/quest.|

## **F.2  Tier Comparison Matrix**

|**Attribute**|**Tier 1 — Flagship**|**Tier 2 — Standard**|**Tier 3 — Basic**|**Tier 4 — Procedural**|
| :-: | :-: | :-: | :-: | :-: |
|Số lượng|~50|~500|~5,000+|Toàn bộ còn lại|
|3D Model|100% custom, interior|Semi-custom, exterior|Asset lib + texture|Auto-gen từ OSM footprint|
|Texture|4K, seasonal variation|2K standard|512-1K shared atlas|512 procedural|
|NPC riêng|Unique NPCs có story|2-3 NPC generic|1 NPC generic|Không NPC|
|Quest liên kết|10+ quests|2-5 quests|1 quest cơ bản|Không quest|
|Nội dung|2000+ từ + voice-over|500-1000 từ text|Tên + địa chỉ + giờ|Tên từ OSM only|
|Audio guide|✅ Có (VI + EN)|❌ Không|❌ Không|❌ Không|
|Photo gallery|30 ảnh reference|10 ảnh reference|1-3 ảnh|Không|
|EXP reward checkin|500-1000 EXP|100-500 EXP|50 EXP|10 EXP|
|LOD cấp|LOD 0-4 (4 mức chi tiết)|LOD 0-3|LOD 0-2|LOD 0-1|

## **F.3  Check-In Flow Validation**

|**Condition Check**|**Pass → Action**|**Fail → Response**|
| :-: | :-: | :-: |
|distance ≤ check\_in\_radius\_m|Proceed to next check|Prompt 'Đến gần hơn để check-in'|
|player chưa checkin POI này|First check-in → 5x EXP bonus|Re-checkin: 10% EXP bình thường, không badge|
|giờ hiện tại trong open\_hours|Proceed|Notify 'Địa điểm đóng cửa. Mở lại lúc [giờ]'|
|requires\_photo = true|Mở camera, yêu cầu chụp ảnh|Bypass check nếu requires\_photo = false|
|Photo AI verify pass|Confirm checkin, stamp journal|Reject: 'Ảnh chưa thấy địa điểm rõ, chụp lại'|
|is\_hidden = true (lần đầu)|Reveal trên map vĩnh viễn cho player này|N/A — không visible để fail|



|**§G — Currency Atom**  |  Tầng Nguyên Tử: Hệ Thống Tiền Tệ|
| :- |

## **G.1  Currency Transaction Spec**
*🚫  CRITICAL: Mọi transaction phải là atomic DB operation (BEGIN/COMMIT). Không bao giờ partial-update balance.*

|**Field**|**VND Game (Soft)**|**Xu Vàng / XV (Hard)**|
| :-: | :-: | :-: |
|Đơn vị|đồng (int64)|xu (int32)|
|Kiếm qua|Quest, nghề, bán ảnh, bán tour, daily login|Mua tiền thật, event đặc biệt, streak 30 ngày|
|Tiêu vào|Thuê xe, ăn uống, lưu trú, tham quan, P2P market|Cosmetic, Vietnam Pass, slot thêm, emote hiếm|
|Max balance|500,000,000đ game (hard cap)|Không giới hạn|
|Expiry|Không hết hạn|2 năm từ ngày mua. Cảnh báo 30 ngày trước.|
|P2P trade|Có, thuế 5% trên transaction|Không (lock, không trade được)|
|Daily P2P cap|Max 10,000,000đ game / ngày|N/A|
|Refund policy|Không refund VND game|Xu Vàng chưa dùng: refund trong 14 ngày|
|Display rule|Luôn hiện HUD, dùng separator (1.500.000đ)|Chỉ hiện khi vào Shop. Không trên HUD game chính.|
|Conversion|Không đổi VND game → XV|Không đổi XV → VND game|

## **G.2  Anti-Inflation Rule Engine**

|**Rule ID**|**Trigger Condition**|**Action**|**Threshold**|
| :-: | :-: | :-: | :-: |
|AI-01|Quest completion rate > 20/ngày/player|Block thêm quest, log anomaly|Strict: không thể bypass|
|AI-02|P2P transaction > 10M đ/ngày/player|Block transaction, return error|Soft: unlock qua GM review|
|AI-03|Balance tăng > 500% baseline nghề / giờ|Freeze wallet 24h, trigger manual review|Nghề × 5x = flag|
|AI-04|Nhận tiền từ > 10 tài khoản khác nhau / ngày|Flag potential smurf ring, suspend + review|Cluster analysis|
|AI-05|Tour price < 50,000đ hoặc > 10,000,000đ|Reject tour listing, return error|Price floor/ceiling|
|AI-06|Ảnh bán với giá < 1,000đ hoặc > 5,000,000đ|Reject listing|Price floor/ceiling|
|AI-07|Giao dịch xảy ra < 200ms sau login (bot pattern)|CAPTCHA, suspend nếu fail|Bot detection|



|**§H — NPC\_AI Atom**  |  Tầng Nguyên Tử: Trí Tuệ Nhân Tạo NPC|
| :- |

## **H.1  3-Layer NPC Architecture**

|**Layer**|**Tên**|**Số lượng**|**AI Engine**|**CPU/NPC**|**Kích hoạt khi**|
| :-: | :-: | :-: | :-: | :-: | :-: |
|L1|NPC Nền|~99,000|FSM cố định, không AI|< 0.1ms|Luôn active khi trong view distance (200m)|
|L2|NPC Quan Trọng|~900|Script + context lookup|0\.5-2ms|Player trong 50m → activate, ngoài 100m → sleep|
|L3|NPC Đặc Biệt|~100|LLM API call (streaming)|API latency|Player initiate dialogue → 1 API call / exchange|

## **H.2  NPC Memory Schema (Layer 2 & 3)**

|**Field**|**Type**|**Layer**|**TTL**|**Mô Tả**|
| :-: | :-: | :-: | :-: | :-: |
|npc\_id|UUID|L2, L3|∞|ID của NPC. Immutable.|
|player\_encounters[]|[]EncounterRecord|L2, L3|7 ngày (L2) / ∞ (L3)|Danh sách gặp gỡ player: timestamp, location, topic|
|last\_conversation|JSON|L3 only|30 ngày|Nội dung cuộc trò chuyện gần nhất|
|relationship\_score|float32 [0-100]|L3 only|∞|0=lạ mặt, 50=quen, 100=thân thiết. Tăng qua tương tác.|
|current\_location|poi\_id | string|L2, L3|realtime|Cập nhật theo schedule. Đổi khi schedule trigger.|
|current\_activity|enum|L2, L3|realtime|WORKING | RESTING | EATING | COMMUTING | SLEEPING|
|mood|enum|L2, L3|Dynamic|HAPPY | NEUTRAL | BUSY | TIRED. Ảnh hưởng dialogue style.|
|knowledge\_tags[]|[]string|L2, L3|∞|Những gì NPC biết. VD: ['local\_food','hidden\_spots','history']|
|dialogue\_script\_id|string | null|L2|∞|null nếu dynamic. string = ID script pre-written.|

## **H.3  LLM Context Window Template (Layer 3 — ≤ 1500 tokens)**
*⚠️  CẢNH BÁO: Context được inject vào mỗi API call. Tuân thủ strict token budget.*

|**Context Block**|**Token Budget**|**Example Content**|**Source**|
| :-: | :-: | :-: | :-: |
|System role|100|"Bạn là [Tên NPC], [tuổi] tuổi, [nghề], người [vùng]. Tính cách: [traits]. Giọng: [accent]."|NPC profile DB|
|World state|150|"Hiện tại: 22:15 thứ Tư, mùa mưa, đang mưa to tại Đà Nẵng. Sự kiện đang có: Hội chợ ẩm thực cuối tuần."|World server|
|NPC location|50|"Vị trí: Quán Bún Bò 48 Trần Phú. NPC đang: phục vụ khách, bận vừa."|NPC schedule|
|Player history|200|"Người chơi [tên] đã gặp NPC: 2 lần. Lần 1: 3 ngày trước hỏi đường. Lần 2: hôm qua mua bún."|Memory DB|
|Player profile|100|"Người chơi: Explorer Lv 15, Cultural Knowledge 120, hiện có buff Phở Bò."|Player DB|
|Player message|100|Input thực tế của player trong lượt này.|Realtime|
|Response format|100|"Trả lời ngắn gọn (< 3 câu) theo giọng địa phương. Không hỏi quá 1 câu ngược lại."|System prompt|
|Buffer dư|700|Giữ headroom cho chain-of-thought LLM.|Safety margin|

## **H.4  NPC Daily Schedule (Layer 2 — 24h)**

|**Giờ**|**Activity**|**Location**|**Dialogue Style**|**Interaction Level**|
| :-: | :-: | :-: | :-: | :-: |
|05:30-07:00|Tập thể dục / Chợ sáng|Công viên / Chợ|Vui vẻ, chào buổi sáng|FULL — sẵn sàng nói chuyện|
|07:00-09:00|Đi làm / Mở hàng|Đường phố / Cửa hàng|Bận, ngắn gọn|PARTIAL — câu ngắn|
|09:00-11:30|Làm việc chính|Vị trí làm việc|Thoải mái, đầy đủ|FULL|
|11:30-13:00|Nghỉ trưa|Nhà / Quán cơm|Mời cơm nếu gặp|PARTIAL — đang nghỉ|
|13:00-17:00|Làm chiều|Vị trí làm việc|Bình thường|FULL|
|17:00-18:30|Tan làm / Về nhà|Đường về nhà|Vội vàng|MINIMAL — câu rất ngắn|
|18:30-21:00|Ăn tối / Cà phê|Nhà hàng / Quán|Thư giãn, nhiều chuyện|FULL — tốt nhất để nói chuyện sâu|
|21:00-23:00|Giải trí / Nhà|Nhà|Bán formal nếu gặp|PARTIAL|
|23:00+|Ngủ|Nhà (invisible)|'Ngủ rồi, hỏi mai nhé'|NONE — auto-dismiss|



|**§I — ChatMessage Atom**  |  Tầng Nguyên Tử: Hệ Thống Chat|
| :- |

## **I.1  ChatMessage Data Model**

|**Field**|**Type**|**Constraint**|
| :-: | :-: | :-: |
|msg\_id|UUID|Auto-gen. Immutable.|
|channel|enum|PROXIMITY | PARTY | ZONE | WHISPER | GUILD|
|sender\_id|player\_id ref|Must be valid active player.|
|content|string|Max 500 chars PROXIMITY/WHISPER, 200 chars ZONE, 1000 chars PARTY/GUILD.|
|timestamp|int64 (unix ms)|Server time. Client time rejected.|
|expiry\_seconds|int32|86400 (24h) cho PROXIMITY. ∞ cho GUILD (7 ngày lịch sử).|
|filter\_status|enum|CLEAN | FLAGGED | BLOCKED. Auto-filter chạy ngay khi nhận.|
|is\_bubble\_3d|bool|true = hiện bubble trên đầu nhân vật 3D. false = chỉ chat panel.|

## **I.2  Rate Limits Per Channel**

|**Channel**|**Max Tin / Giây**|**Max Ký Tự / Tin**|**Cooldown Vi Phạm**|
| :-: | :-: | :-: | :-: |
|PROXIMITY|3 tin / 10 giây|500|Mute 30s nếu spam 10 tin/30s|
|PARTY|Không giới hạn tần suất|1000|Chỉ bị limit nếu flood > 10 tin/giây|
|WHISPER|2 tin / 5 giây|500|Mute 60s nếu DM spam|
|ZONE|1 tin / 30 giây|200|Không thể bypass. Hard limit.|
|GUILD|Không giới hạn|1000 (lịch sử 7 ngày)|Moderate by Guild Leader|



|**§J — Quest Atom**  |  Tầng Nguyên Tử: Hệ Thống Nhiệm Vụ|
| :- |

## **J.1  QuestEntry Data Model**

|**Field**|**Type**|**Mô Tả**|
| :-: | :-: | :-: |
|quest\_id|string (QUEST-XXXX)|ID định danh. Ví dụ: QUEST-0042-BA-NAM-MEO.|
|quest\_type|enum|MAIN | SIDE | DAILY | WEEKLY | COMMUNITY | HIDDEN | SEASONAL|
|status|enum|LOCKED | AVAILABLE | ACTIVE | COMPLETED | FAILED | EXPIRED|
|zone\_id|string | null|null = toàn quốc. 'HOI\_AN' = chỉ ở Hội An.|
|giver\_npc\_id|npc\_id | null|null = system quest (auto-assign). Non-null = từ NPC.|
|steps[]|[]QuestStep|Danh sách các bước. Quest xong khi tất cả steps DONE.|
|reward\_vnd|int64|Tiền thưởng. 0 nếu không có.|
|reward\_exp\_type|enum|EXPLORER | SOCIAL | CULTURAL | PHOTO | PROFESSION|
|reward\_exp\_amount|int32|Số EXP theo type.|
|reward\_items[]|[]ItemRef|Danh sách item. Rỗng nếu không có.|
|choice\_branches|JSON | null|null nếu tuyến tính. JSON nếu có nhánh lựa chọn.|
|consequence\_npc\_state|JSON | null|NPC state thay đổi sau quest. VD: bà Năm buồn 3 ngày.|
|cooldown\_hours|int32 | null|null = không reset. 24 = daily. 168 = weekly.|
|max\_active\_same\_type|int32|Tối đa quest cùng loại active cùng lúc. DAILY: 3.|

## **J.2  4C Framework — Checklist Validate Mọi Quest Mới**
*🚫  CRITICAL: Quest KHÔNG được implement nếu chưa qua đủ 4 checklist dưới đây.*

|**C**|**Câu Hỏi Validate**|**Pass Criteria**|**Ví Dụ (Quest Bà Năm Mất Mèo)**|
| :-: | :-: | :-: | :-: |
|CONTEXT|Tại sao quest này tồn tại?|Gắn với NPC thật / địa điểm thật / sự kiện thật của thế giới|Bà Năm là NPC chủ tạp hóa thật tại Hội An, có schedule, có memory|
|CHALLENGE|Player phải làm gì cụ thể?|Action rõ ràng, không mơ hồ. Có thể đo được.|Tìm mèo trong bán kính 200m chợ, hỏi 3 NPC để lần dấu vết|
|CHOICE|Có ≥ 2 cách giải quyết?|Ít nhất 1 branching point. Cả 2 cách đều valid.|Cách 1: Tự tìm (EXP +50%). Cách 2: Nhờ NPC (tốn 20k, nhanh hơn).|
|CONSEQUENCE|Lựa chọn có ảnh hưởng thật?|Outcome khác nhau thật sự. Không phải illusion.|Mèo chết → bà Năm buồn 3 ngày game (dialogue thay đổi, shop giảm item)|

## **J.3  Progression Multi-Dimension — Threshold Gates**

|**Chỉ Số**|**Level / Score**|**Unlocks**|**Cách Tăng**|
| :-: | :-: | :-: | :-: |
|Explorer Level|Lv 10|Hidden zone category unlock|Checkin địa điểm mới, quest khám phá|
|Explorer Level|Lv 30|NPC chia sẻ secret locations (tier 1)|Tích lũy dần|
|Explorer Level|Lv 50|Title 'Phượt Thủ Chuyên Nghiệp'|Milestone tự động|
|Explorer Level|Lv 100|Title 'Phượt Thủ Huyền Thoại', avatar frame hiếm nhất|Endgame achievement|
|Cultural Knowledge|≥ 50|NPC L2 bắt đầu chia sẻ thông tin nâng cao|Đọc lịch sử POI, quiz, hội thoại|
|Cultural Knowledge|≥ 200|Profession Hướng Dẫn Viên Lv 5 unlock|Tích lũy qua tham quan T1/T2|
|Cultural Knowledge|≥ 500|Access hidden dialogue branch L3 NPC|Chỉ từ T1 POI + L3 NPC conversation|
|Reputation|≥ 3.5 ⭐|Badge xanh 'Đáng Tin' trên profile public|Review 2 chiều sau dịch vụ|
|Reputation|≥ 4.5 ⭐ (50+ reviews)|Badge vàng 'Xuất Sắc', giảm giá NPC 10%|Dịch vụ nhất quán chất lượng cao|
|Traveler Stamp|63 / 63|Title 'Người Việt Toàn Quốc' + Avatar frame đặc biệt|Đến đủ 63 tỉnh/thành|



|**§K — PhotoScore Atom**  |  Tầng Nguyên Tử: Hệ Thống Nhiếp Ảnh|
| :- |

## **K.1  PhotoEntry Schema**

|**Field**|**Type**|**Constraint**|
| :-: | :-: | :-: |
|photo\_id|UUID|Auto-gen khi chụp.|
|photographer\_id|player\_id|Immutable. Xác định tác quyền.|
|location\_poi\_id|poi\_id | null|null nếu không gắn với POI cụ thể.|
|timestamp|int64 unix ms|Server time. Dùng để tính 'giờ vàng' bonus.|
|elo\_score|int32|Bắt đầu 1000-1500 (AI auto-score). Thay đổi theo community.|
|technical\_score|float32 [0-100]|AI auto: brightness (30%) + sharpness (30%) + composition (40%).|
|community\_likes|int32|Mỗi like: +15 ELO nếu liker có photo\_score > 1000.|
|community\_dislikes|int32|Mỗi dislike: −5 ELO. Weighted ít hơn để tránh abuse.|
|times\_purchased|int32|Mỗi lần mua: +50 ELO + 80% giá về author.|
|for\_sale|bool|Player chọn có/không bán. Giá tự đặt (floor 1k, cap 5M).|
|sale\_price|int64 | null|null nếu not for\_sale. Trong [1000, 5,000,000]đ.|
|featured|bool|true = top 10 ELO → hiện feed chính. Auto-computed.|

## **K.2  ELO Score Mechanics**

|**Event**|**ELO Delta**|**Điều Kiện Đặc Biệt**|
| :-: | :-: | :-: |
|Upload ảnh|+1000 base (AI technical score)|AI score 90+ → start 1500. AI score < 40 → start 800.|
|Like từ player thường|+15|Không hạn chế số lượt like nhận / ngày|
|Like từ player Photo Bậc Thầy+|+30|Nhân đôi vì nguồn uy tín cao hơn|
|Dislike|−5|Max 3 dislike / ảnh / ngày để tránh coordinated attack|
|Ảnh được mua|+50|Cộng thêm bất kể giá bán|
|Ảnh được Featured|+100 (1 lần)|Khi đạt top 10 → featured → ELO +100 ngay|
|Ảnh không like trong 7 ngày|−10 (decay)|Giữ feed tươi mới, tránh tích trữ ảnh cũ|
|Ảnh bị report xấu (pass moderation)|−200 + xóa|Về 0 nếu < 200. Không âm.|
|Buff Cà Phê Trứng active|×1.5 cho 5 ảnh tiếp|Buffer của lần chụp, không của ELO tính sau|



|**§L — WeatherState Atom**  |  Tầng Nguyên Tử: Hệ Thống Thời Tiết|
| :- |

## **L.1  WeatherState Schema**

|**Field**|**Type**|**Mô Tả**|
| :-: | :-: | :-: |
|zone\_id|string|Zone áp dụng weather này.|
|weather\_type|enum|SUNNY\_CLEAR | SUNNY\_HOT | RAIN\_LIGHT | RAIN\_HEAVY | STORM | FOG | CLOUDY|
|temperature\_c|float32|Nhiệt độ Celsius. Ảnh hưởng stamina nếu > 35°C.|
|visibility\_m|int32|Tầm nhìn mét. FOG: 50m. STORM: 100m. CLEAR: ∞|
|wind\_speed\_kmh|float32|Ảnh hưởng tốc độ xe đạp, vật lý cây cối.|
|rain\_intensity|float32 [0-1]|0 = không mưa. 1 = mưa xối xả. Ảnh hưởng NPC behavior.|
|updated\_at|timestamp|Cập nhật từ real weather API (OpenWeatherMap) mỗi 15 phút.|
|override\_active|bool|true = GM đang override thời tiết cho event/debug.|

## **L.2  Weather → Gameplay Effect Matrix**

|**Weather**|**Stamina Effect**|**Vehicle Effect**|**Photo Effect**|**NPC Effect**|**POI Effect**|
| :-: | :-: | :-: | :-: | :-: | :-: |
|SUNNY\_CLEAR|Không|Không|+10% score (ánh sáng đẹp)|Đông tại outdoor|Tất cả mở|
|SUNNY\_HOT (>35°C, 12-14h)|−0.5/s ngoài trời|Không|−5% (overexposed)|NPC nghỉ trưa, vắng|Nhiều POI đóng trưa|
|RAIN\_LIGHT|Không|Speed −10%|Special rain filter +15%|Vào trong nhà|Outdoor POI vắng|
|RAIN\_HEAVY|Không|Speed −30%, dễ trượt|−20% (lens blur)|Hầu hết vào trong|Outdoor event hủy|
|STORM|Không|Speed −50%, taxi +50% giá|Camera shake, unusable|Tất cả vào trong|Máy bay hoãn, tàu ngừng|
|FOG|Không|Speed −20%|+25% lãng mạn style|Di chuyển chậm|Explorer EXP +30%|
|CLOUDY|Không|Không|Diffuse light tốt (portrait)|Bình thường|Bình thường|



|**§N — ServerSync Atom**  |  Tầng Nguyên Tử: Đồng Bộ Máy Chủ|
| :- |

## **N.1  Server Tick & Sync Parameters**

|**Parameter**|**Value**|**Unit**|**Lý Do / Trade-off**|
| :-: | :-: | :-: | :-: |
|game\_server\_tick|20|tick/giây|Đủ mượt cho game khám phá. Không cần 60Hz như FPS.|
|world\_state\_tick|5|tick/giây|Thời tiết, NPC schedule, event — không cần nhanh.|
|player\_position\_sync|50|ms interval|Gửi qua UDP Reliable. 20 updates/giây.|
|max\_ccu\_per\_zone\_instance|200|players|Trên 200 → auto-spawn instance mới cùng zone.|
|interest\_mgmt\_radius|200|m game|Chỉ nhận update từ players trong 200m → −80% bandwidth.|
|dead\_reckoning\_window|200|ms|Client tự predict ≤ 200ms. Quá 200ms → visual glitch.|
|checkpoint\_save\_interval|30|giây|Server crash → mất tối đa 30 giây progress.|
|reconnect\_grace\_period|60|giây|Disconnect → 60 giây để reconnect giữ session.|
|idle\_zone\_sleep\_timeout|300|giây|Zone instance không có player 5 phút → sleep (giảm cost).|

## **N.2  Network Protocol Per Data Type**

|**Data Type**|**Protocol**|**Direction**|**Reliability**|**Lý Do Chọn**|
| :-: | :-: | :-: | :-: | :-: |
|Player position / rotation|UDP (EOS Reliable UDP)|C→S, S→C|Best effort|Tốc độ > accuracy. Mất 1 packet OK.|
|Chat text|WebSocket (TCP persistent)|C→S, S→C|Guaranteed order|Không được mất tin, thứ tự quan trọng.|
|Quest / inventory / balance|HTTPS REST|C→S (action), S→C (confirm)|Guaranteed|Stateless, retry-safe, cacheable, dễ audit.|
|NPC L3 AI response|HTTPS + SSE (streaming)|S→C|Guaranteed (SSE reconnect)|Stream từng token như ChatGPT → UX mượt hơn.|
|Push notifications|WebSocket persistent|S→C only|Guaranteed|Server push không cần client poll.|
|Map tiles / 3D assets|HTTP/2 + CDN Cloudflare|S→C|N/A (cacheable)|CDN edge tại VN, multiplexed, compressed.|
|Auth / login|HTTPS REST + JWT|C→S|Guaranteed|Standard security. JWT 15min + refresh 7 ngày.|
|Voice chat|Vivox DTLS-SRTP|P2P mediated|Real-time|Spatial audio, < 150ms latency target.|



|**§P — UIComponent Atom**  |  Tầng Nguyên Tử: Thành Phần Giao Diện|
| :- |

## **P.1  HUD Layout Specification**

|**Zone**|**Vị Trí**|**Components**|**Size (PC)**|**Ẩn Khi**|
| :-: | :-: | :-: | :-: | :-: |
|NW (Trái Trên)|Top-left|Minimap (bản đồ giấy cổ), la bàn, GPS text|128×128px|Không bao giờ ẩn hoàn toàn (chỉ thu nhỏ)|
|NE (Phải Trên)|Top-right|Game clock (24h), weather icon, zone name|Auto-width|Ẩn trong Photo Mode|
|SW (Trái Dưới)|Bottom-left|Active quest (1-2 dòng), progress %|300px width|Ẩn khi không có quest active (slide out 0.3s)|
|SE (Phải Dưới)|Bottom-right|VND balance, stamina bar, ping ms|Auto-size|Ẩn hoàn toàn trong Photo Mode|
|SC (Giữa Dưới)|Bottom-center|Action bar: xe, hotel, food, map, friends|80% width PC|Ẩn sau 8s idle, hiện khi hover/tap|
|CC (Giữa Màn)|Center|Crosshair + interaction tooltip|Dot + tooltip|Chỉ hiện khi nhìn interactable object < 5m|

## **P.2  Color System — Design Tokens**

|**Token Name**|**Hex**|**RGB**|**Dùng Cho**|**KHÔNG Dùng Cho**|
| :-: | :-: | :-: | :-: | :-: |
|--color-primary|#1B6CA8|27, 108, 168|Primary action, links, minimap border|Error states, warnings|
|--color-gold|#C8860A|200, 134, 10|Xu Vàng, achievement, starred items|Negative feedback|
|--color-success|#1A7A3C|26, 122, 60|Buff active, positive rep, success state|Error, danger|
|--color-danger|#C0392B|192, 57, 43|Error, ban warning, stamina < 20%|Success, neutral|
|--color-bg|#FAF7F2|250, 247, 242|Background UI panels (light mode)|Dark mode (dùng --color-dark)|
|--color-text|#1A1A2E|26, 26, 46|Primary text. Không dùng #000000 thuần|Background|
|--color-overlay|rgba(0,0,0,0.6)|—|Modal backdrop, dimmed world|Không dùng solid black|

## **P.3  Animation Performance Rules**
*🚫  CRITICAL: Tất cả UI animation phải dùng CSS transform/opacity (GPU compositing). KHÔNG dùng width/height/top/left animation.*

|**Component**|**Animation Type**|**Duration**|**Easing**|**GPU Property**|
| :-: | :-: | :-: | :-: | :-: |
|HUD slide in/out|translateY()|300ms|ease-out|transform ✅|
|Panel modal open|scale(0.9→1) + opacity(0→1)|250ms|ease-out-back|transform + opacity ✅|
|Quest notification|translateX(-100%→0)|400ms|spring(0.8)|transform ✅|
|Buff icon appear|scale(0→1) + opacity|200ms|ease-out|transform + opacity ✅|
|Minimap pulse|opacity(1→0.4→1)|1000ms loop|ease-in-out|opacity ✅|
|❌ FORBIDDEN|width animation|—|—|width ❌ (layout reflow)|
|❌ FORBIDDEN|top/left animation|—|—|top/left ❌ (layout reflow)|
|❌ FORBIDDEN|height animation|—|—|height ❌ (layout reflow)|



|**§Q — OnboardingEvent Atom**  |  Tầng Nguyên Tử: Luồng Người Chơi Mới|
| :- |

## **Q.1  Minute-by-Minute FTUE Timeline**
*ℹ️  GHI CHÚ KỸ THUẬT: FTUE = First Time User Experience. Mỗi giây của 5 phút đầu tiên đều được thiết kế có chủ đích.*

|**Thời Điểm**|**Sự Kiện**|**Learning Target**|**Cảm Xúc Mục Tiêu**|**Skip?**||
| :-: | :-: | :-: | :-: | :-: | :- |
|0:00–0:30|Logo → Cinematic 30s flycam Hội An bình minh, nhạc đàn tranh, ZERO text|Tạo mong đợi, không overwhelm|Ngạc nhiên, tò mò|Không||
|0:30–1:30|Character Creator: chỉ 3 lựa chọn (giới tính, tone da, tên). Nút 'Tùy chỉnh thêm' ẩn.|Sở hữu nhân vật, không overwhelm|Hứng khởi, cá nhân|Không||
|1:30–2:00|Cutscene ngắn: bước xuống xe khách đến Hội An. NPC: 'Chào mừng bạn!'|Biết mình đang ở đâu, tại sao đến|Ấm áp, được chào đón|Có (skip button)||
|2:00–3:30|Tutorial đi bộ: tooltip nhỏ WASD. NPC đồng hành dẫn đến điểm gần.|Học di chuyển tự nhiên, không tutorial box|Tự tin, kiểm soát|Không||
|3:30–4:30|Đến Chùa Cầu: NPC kể 1 câu ngắn. Nút 'Check-in' nhấp nháy → nhấn → +200k VND + EXP|Check-in flow, nhận reward đầu tiên|Vui vẻ, muốn làm tiếp|Không||
|4:30–5:30|Quest đầu tiên tự pop: 'Chụp 3 ảnh Phố Cổ'. Minimap gợi ý 3 điểm. Không timer.|Quest system + photo system, tự do|Tự do, thoải mái|Không||
|5:30+|Tutorial kết thúc. Popup nhỏ: 'Thế giới đang chờ bạn!' → World mở hoàn toàn.|Transition tự do exploration|Phấn khích, muốn tiếp tục|N/A||



|**§R — RetentionTrigger Atom**  |  Tầng Nguyên Tử: Cơ Chế Giữ Chân|
| :- |

## **R.1  3-Loop Retention Framework**

|**Loop**|**Chu Kỳ**|**Cơ Chế Chính**|**KPI Target**|**Failure Sign**|
| :-: | :-: | :-: | :-: | :-: |
|Loop 1 — Daily Hook|Mỗi ngày (10-20 phút)|Daily quest ×3, login reward, thời tiết thay đổi, NPC nhớ tên bạn|D7 retention > 30%|D7 < 20% → redesign daily quest|
|Loop 2 — Weekly Driver|Mỗi tuần (1-3 giờ)|Weekly challenge, event cộng đồng, streak bonus, leaderboard reset thứ 2|D30 retention > 25%|D30 < 15% → thêm weekly social event|
|Loop 3 — Long-term Arc|Hàng tháng / Mùa|Zone mới, seasonal event, milestone 63 tỉnh, B2B partnership events|D90 retention > 15%|D90 < 10% → audit content depth|

## **R.2  Churn Signal → Auto-Response Table**

|**Signal**|**Detect Condition**|**Delay**|**Auto Action**|**Human Review?**||
| :-: | :-: | :-: | :-: | :-: | :- |
|3-day absent|Login gap > 72h|Realtime|Push: 'NPC [tên quen] đang hỏi thăm bạn!'|Không||
|7-day absent|Login gap > 168h|Realtime|Email cá nhân hóa: best photo + sự kiện đang có|Không||
|Short sessions|Session < 5min, 3 ngày liên tiếp|24h after 3rd|Gợi ý tính năng chưa thử, quest phù hợp profile|Không||
|Social isolation|0 friends sau ngày 7|Day 7 trigger|Gợi ý join guild, giới thiệu 3 người cùng zone|Không||
|Broke + no quest|Balance < 5k + 0 active quest|Realtime|Auto-assign daily quest cao reward, tip kiếm tiền|Không||
|Anomalous drop|DAU giảm > 20% 1 ngày|Hourly check|Alert team → check server health, content issue|Có — team review||

## **R.3  Daily Login Streak Reward Table**
*✅  BEST PRACTICE: Streak KHÔNG reset nếu miss ≤ 3 ngày. 1 Streak Shield miễn phí / tuần.*

|**Ngày**|**Reward**|**Ghi Chú**|
| :-: | :-: | :-: |
|Ngày 1|100,000đ game|Khởi đầu nhỏ — không gây áp lực|
|Ngày 2|200,000đ + 1 Stamina Boost (2h)|Tăng dần giá trị|
|Ngày 3|300,000đ|Tuyến tính tăng|
|Ngày 4|Random cosmetic nhỏ (filter / sticker)|Surprise element — tạo anticipation|
|Ngày 5 ⭐|500,000đ + 5 Xu Vàng|Milestone đầu tiên có XV — quan trọng|
|Ngày 7 ⭐⭐|1,000,000đ + limited item tuần + 10 XV|Streak bonus lớn nhất tuần — bảo vệ bằng Shield|
|Ngày 30 ⭐⭐⭐|Cosmetic hiếm + Title 'Người Thường Trú' + 50 XV|Milestone tháng — motivation dài hạn lớn nhất|



|**APPENDIX — Design Litmus Test**  |  5 Câu Hỏi Bắt Buộc Trước Mọi Feature|
| :- |

*🚫  CRITICAL: Đây là check bắt buộc cho BẤT KỲ atom/component nào mới. Nếu câu trả lời là KHÔNG → từ chối hoặc redesign.*

|**#**|**Câu Hỏi**|**Pillar**|**Nếu KHÔNG →**|
| :-: | :-: | :-: | :-: |
|Q1|Atom/Feature này phục vụ ít nhất 1 trong 5 Design Pillar?|Tất cả|Từ chối hoàn toàn. Không có exception.|
|Q2|Người chơi miễn phí có trải nghiệm được không?|RESPECT|Xem xét lại monetization. Nguy cơ P2W.|
|Q3|Feature phản ánh Việt Nam chân thực?|AUTHENTICITY|Fact-check hoặc mời chuyên gia văn hóa tư vấn.|
|Q4|Có thể giải thích feature trong vòng 10 giây không?|RESPECT|Quá phức tạp — đơn giản hóa trước khi build.|
|Q5|Khi chơi cùng người khác, feature tốt hơn không?|CONNECTION|Đánh giá lại tính xã hội. Ưu tiên thấp hơn.|

## **5 Design Pillars — Quick Reference**

|**Pillar**|**Tagline**|**Biểu Hiện Cốt Lõi**|**Anti-Pattern**|
| :-: | :-: | :-: | :-: |
|1️⃣ AUTHENTICITY|Việt Nam thật, không lý tưởng hóa|OSM data thật, giá tham chiếu thật, NPC giọng vùng thật|Thêm tòa nhà không có thật để 'trông đẹp hơn'|
|2️⃣ CONNECTION|Tốt hơn khi chơi cùng người|Mọi mechanic có version multiplayer tốt hơn solo|Nội dung buộc chơi một mình để hoàn thành|
|3️⃣ DISCOVERY|Luôn có điều mới để tìm thấy|63 tỉnh khác nhau, hidden locations, NPC theo giờ|Map lộ hoàn toàn từ đầu, content lặp|
|4️⃣ RESPECT|Tôn trọng người chơi & văn hóa|Không dark pattern, không punish, fact-check lịch sử|Push notification spam, pay wall content chính|
|5️⃣ SUSTAINABILITY|Bền vững cho cả người chơi & studio|F2P đầy đủ, B2B tourism revenue, UGC giảm cost|Pay-to-Win, chi phí content tuyến tính với growth|

|<p>Vietnam Online — Atomic / Component / Detail Level  ·  v1.0</p><p>*"Trở thành cánh cửa số dẫn mọi người đến Việt Nam — trước khi họ đặt chân đến thật."*</p>|
| :-: |

Trang  / 
