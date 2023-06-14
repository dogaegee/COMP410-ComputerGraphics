using UnityEngine;
using UnityEngine.UI;
using TMPro;
public class CounterManager : MonoBehaviour
{
    public TextMeshProUGUI countdownText;
    private int countdownValue = 5;

    private void Start()
    {
        StartCoroutine(StartCountdown());
    }

    private System.Collections.IEnumerator StartCountdown()
    {
        while (countdownValue > 0)
        {
            countdownText.text = countdownValue.ToString();
            yield return new WaitForSeconds(1f);
            countdownValue--;
        }

        countdownText.text = "Go!";
        countdownText.color = Color.green;
        yield return new WaitForSeconds(1f);
        countdownText.enabled = false;
        // Add any desired actions after the countdown finishes
        
    }
}
