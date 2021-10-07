import datetime

info = datetime.datetime.now()
weekday = info.weekday()
day = info.day
hour = info.hour
minute = info.minute
str1 = ""
str2 = ""
next_hour = 0
next_min = 0

def timediff(target_hour, target_mins):
    curr_mins = hour * 60 + minute
    target_mins = target_hour * 60 + target_mins
    diff_mins = target_mins - curr_mins
    diff_hours = int(diff_mins / 60)
    diff_mins = diff_mins % 60
    return diff_hours, diff_mins

if weekday == 0 or weekday == 2: #MW
    if hour < 10:
        str1, str2 = "Pre", "Physics 4AL"
        next_hour, next_min = 10, 0
    elif hour == 10 or (hour == 11 and minute < 50):
        str1, str2 = "Physics 4AL", "Break"
        next_hour, next_min = 11, 50
    elif hour == 11:
        str1, str2 = "Break", "Math 33A"
        next_hour, next_min = 12, 0
    elif hour == 12 and minute < 50:
        str1, str2 = "Math 33A", "Break"
        next_hour, next_min = 12, 50
    elif hour < 16:
        str1, str2 = "Break", "Physics 1B"
        next_hour, next_min = 16, 0
    elif hour == 16 and minute < 50:
        str1, str2 = "Physics 1B", "Post"
        next_hour, next_min = 16, 50
    else:
        str1, str2 = "Post", "Pre"
        next_hour, next_min = 24, 0
elif weekday == 1: #T
    if hour < 10:
        str1, str2 = "Pre", "CS M51A"
        next_hour, next_min = 10, 0 
    elif hour == 10 or (hour == 11 and minute < 50):
        str1, str2 = "CS M51A", "Break"
        next_hour, next_min = 11, 50
    elif hour < 16:
        str1, str2 = "Break", "Physics 1B"
        next_hour, next_min = 16, 0
    elif hour == 16 and minute < 50:
        str1, str2 = "Physics 1B", "Post"
        next_hour, next_min = 16, 50
    else:
        str1, str2 = "Post", "Pre"
        next_hour, next_min = 24, 0
elif weekday == 3: #R
    if hour < 9:
        str1, str2 = "Pre", "Physics 1B Disc"
        next_hour, next_min = 9, 0
    elif hour == 9 and minute < 50:
        str1, str2 = "Physics 1B Disc", "Break"
        next_hour, next_min = 9, 50
    elif hour == 9:
        str1, str2 = "Break", "CS M51A"
        next_hour, next_min = 10,0 
    elif hour == 10 or (hour == 11 and minute < 50):
        str1, str2 = "CS M51A", "Break"
        next_hour, next_min = 11, 50
    elif hour == 11:
        str1, str2 = "Break", "Math 33A Disc"
        next_hour, next_min = 12, 0
    elif hour == 12 and minute < 50:
        str1, str2 = "Math 33A Disc", "Break"
        next_hour, next_min = 12, 50
    elif hour < 18:
        str1, str2 = "Break", "ICPC Interview Prep"
        next_hour, next_min = 18, 0
    elif hour < 20:
        str1, str2 = "ICPC Interview Prep", "Post"
        next_hour, next_min = 20, 0
    else:
        str1, str2 = "Post", "Pre"
        next_hour, next_min = 24, 0
elif weekday == 4: #F
    if hour < 12:
        str1, str2 = "Pre", "Math 33A"
        next_hour, next_min = 12, 0
    elif hour == 12 and minute < 50:
        str1, str2 = "Math 33A", "Break"
        next_hour, next_min = 12, 50
    elif hour < 14:
        str1, str2 = "Break", "CS M51A Disc"
        next_hour, next_min = 14, 0
    elif hour == 14 or (hour == 15 and minute < 50):
        str1, str2 = "CS M51A Disc", "Break"
        next_hour, next_min = 15, 50
    elif hour == 15:
        str1, str2 = "Break", "Physics 1B"
        next_hour, next_min = 16, 0
    elif hour == 16 and minute < 50:
        str1, str2 = "Physics 1B", "Break"
        next_hour, next_min = 16, 50
    elif hour < 18:
        str1, str2 = "Break", "AI Beginner Track"
        next_hour, next_min = 18, 0
    elif hour < 20:
        str1, str2 = "AI Beginner Track", "Post"
        next_hour, next_min = 20, 0
    else:
        str1, str2 = "Post", "Weekend"
        next_hour, next_min = 24, 0
else: #SS
    str1, str2 = "Weekend", "Pre"
    next_hour = 48 - (weekday-5)*24
    next_min = 0

print(info.strftime("Today Is: %A, %B " + str(day) + ", %Y"))
print(info.strftime("The Time Is: %I:%M %p"))
print("Current Activity: %s" % (str1))
print("Next Activity: %s" % (str2))
x, y = timediff(next_hour, next_min)
print("Time Until Next Activity: %s hour(s) and %s minute(s)" % (x, y))
