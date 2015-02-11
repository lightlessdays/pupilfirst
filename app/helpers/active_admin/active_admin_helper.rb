module ActiveAdmin::ActiveAdminHelper
  def sv_id_link(user)
    if user.present?
      link_to "#{user.email} - #{user.fullname} #{user.phone.present? ? "(#{user.phone}" : ''})", admin_user_path(user)
    else
      '<em>Missing, probably deleted.</em>'.html_safe
    end
  end

  def startup_link(startup)
    if startup.present?
      link_to startup.name, admin_startup_path(startup)
    else
      '<em>Unknown</em>'.html_safe
    end
  end

  def availability_as_string(availability)
    day = case availability["days"]
      when Date::DAYNAMES then "Everyday"
      when Date::DAYNAMES[1..5] then "Weekdays"
      when Date::DAYNAMES - Date::DAYNAMES[1..5] then "Weekends"
      else availability["days"]
    end
    time = "#{availability["time"]["after"]}:00 to #{availability["time"]["before"]}:00 hrs"
    "#{day} , #{time}"
  end

end
