module EngineeringMetrics
  class GithubStatsService
    def initialize
      @token = ENV.fetch('GITHUB_ACCESS_TOKEN')
    end

    AUTHORS = %w(harigopal ajaleelp mahesh-sv vinutv).freeze

    def contributions
      contributions = fetch('repos/SVdotCO/sv.co/stats/contributors')
      contributions.reject! { |c| !c['author']['login'].in?(AUTHORS) }
      contributions.map { |c| pretty_contribution(c) }
    end

    # returns commits per founder per week for last 10 weeks
    def commits_trend
      contributions.each_with_object({}) do |developer_stats, result|
        result[developer_stats[:name].to_sym] = developer_stats[:recent_weeks].map { |_k, a| a[:commits] }
      end
    end

    private

    def fetch(path)
      url = 'https://api.github.com/' + path
      JSON.parse(RestClient.get(url, Authorization: "token #{@token}"))
    end

    def pretty_contribution(user_contribution)
      {
        name: user_contribution['author']['login'],
        total_commits: user_contribution['total'],
        recent_weeks: recent_activity(user_contribution['weeks'])
      }
    end

    def recent_activity(contribution)
      contribution[-10..-1].each_with_object({}) do |week, result|
        result[week['w']] = { additions: week['a'], deletions: week['d'], commits: week['c'] }
      end
    end
  end
end
