using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class GameEndController : MonoBehaviour
{
    public TMP_Text endText; // Reference to the text object


    private void Start()
    {
        // Disable the text object initially
        endText.enabled = false;
    }

    private void Update()
    {
        // Check if the score is lower than zero
        if (ScoreManager.Instance.GetScore() <= 0)
        {
            // Show the text object
             endText.enabled = true;
        }
    }
}
