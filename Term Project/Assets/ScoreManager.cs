using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class ScoreManager : MonoBehaviour
{
    private static ScoreManager instance; // Singleton instance

    private int score = 1000; // Starting score
    private TMP_Text scoreText;

    // Public property to access the singleton instance
    public static ScoreManager Instance
    {
        get { return instance; }
    }

    private void Awake()
    {
        if (instance != null && instance != this)
        {
            // Destroy duplicate instances
            Destroy(gameObject);
            return;
        }

        instance = this;
        DontDestroyOnLoad(gameObject);

        scoreText = GetComponent<TMP_Text>();
        UpdateScoreText();
    }

    public void UpdateScore(int points)
    {
        score += points;
        UpdateScoreText();
    }

    public int GetScore()
    {
        return score;
    }

    private void UpdateScoreText()
    {
        if (score <= 0)
        {
            scoreText.color = Color.red;
        }

        scoreText.text = score.ToString();
    }
}
