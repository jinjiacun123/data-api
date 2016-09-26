define qquit
  set confirm off
  quit
end
document qquit
Quit without asking for confirmation.
end
break main
break client_parse_time_share
break client_parse_history 
set args --r 1
r
